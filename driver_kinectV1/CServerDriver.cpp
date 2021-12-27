#include "stdafx.h"

#include "CServerDriver.h"
#include "CEmulatedDevice.h"
#include "CTrackerVive.h"
#include "CKinectStation.h"
#include "CKinectHandler.h"

#include "CDriverConfig.h"
#include "MathUtils.h"
#include "Utils.h"

extern char g_modulePath[];

const std::vector<std::string> g_messageNames
{
    "calibration", "tracking_toggle", "tracker_toggle", "interpolation"
};
enum MessageIndex : size_t
{
    MI_Calibration = 0U,
    MI_TrackingToggle,
    MI_TrackerToggle,
    MI_Interpolation
};

enum HistoryIndex : size_t
{
    HI_Previous = 0U,
    HI_Last,

    HI_Count
};

const char* const CServerDriver::ms_interfaces[]
{
    vr::ITrackedDeviceServerDriver_Version,
        vr::IServerTrackedDeviceProvider_Version,
        nullptr
};

CServerDriver::CServerDriver()
{
    m_kinectHandler = nullptr;
    m_kinectStation = nullptr;
    m_kinectThread = nullptr;
    m_kinectActive = false;
    m_frameHistoryCount = 0U;
    m_basePosition = glm::vec3(0.f);
    m_baseRotation = glm::quat(1.f, 0.f, 0.f, 0.f);
    m_interpolation = CDriverConfig::FI_Linear;
    m_trackingState = true;
    m_dashboardLaunchTick = 0U;
    m_dashboardLaunched = false;
}

CServerDriver::~CServerDriver()
{
}

// vr::IServerTrackedDeviceProvider
vr::EVRInitError CServerDriver::Init(vr::IVRDriverContext *pDriverContext)
{
    VR_INIT_SERVER_DRIVER_CONTEXT(pDriverContext);
    CDriverConfig::Load();

    m_basePosition = CDriverConfig::GetBasePosition();
    m_baseRotation = CDriverConfig::GetBaseRotation();
    m_trackingState = CDriverConfig::GetTrackingState();
    m_interpolation = CDriverConfig::GetInterpolation();

    m_kinectHandler = new CKinectHandler();
    m_kinectActive = true;
    m_kinectThread = new std::thread(&CServerDriver::KinectProcess, this);

    // Add trackers
    for(auto &l_tracker : m_trackers) l_tracker = nullptr;
    for(auto l_index : CDriverConfig::GetBoneIndexes())
    {
        m_trackers[l_index] = new CTrackerVive(l_index);
        m_trackers[l_index]->SetForcedConnected(m_trackingState);
        m_trackers[l_index]->SetOffsetPosition(m_basePosition.x, m_basePosition.y, m_basePosition.z);
        m_trackers[l_index]->SetOffsetRotation(m_baseRotation.x, m_baseRotation.y, m_baseRotation.z, m_baseRotation.w);
        vr::VRServerDriverHost()->TrackedDeviceAdded(m_trackers[l_index]->GetSerial().c_str(), vr::TrackedDeviceClass_GenericTracker, m_trackers[l_index]);
    }

    // Add fake station as command relay with Kinect model
    m_kinectStation = new CKinectStation(this);
    m_kinectStation->SetPosition(m_basePosition.x, m_basePosition.y, m_basePosition.z);
    m_kinectStation->SetRotation(m_baseRotation.x, m_baseRotation.y, m_baseRotation.z, m_baseRotation.w);
    vr::VRServerDriverHost()->TrackedDeviceAdded(m_kinectStation->GetSerial().c_str(), vr::TrackedDeviceClass_TrackingReference, m_kinectStation);

    m_dashboardLaunchTick = GetTickCount64();

    return vr::VRInitError_None;
}

void CServerDriver::Cleanup()
{
    for(auto &l_tracker : m_trackers)
    {
        delete l_tracker;
        l_tracker = nullptr;
    }

    delete m_kinectStation;
    m_kinectStation = nullptr;

    if(m_kinectThread)
    {
        m_kinectActive = false;
        m_kinectThread->join();
        m_kinectThread = nullptr;
    }

    delete m_kinectHandler;
    m_kinectHandler = nullptr;

    for(auto l_frame : m_frameHistory) delete l_frame;
    m_frameHistory.clear();
    m_frameHistoryCount = 0U;

    VR_CLEANUP_SERVER_DRIVER_CONTEXT();
}

const char* const* CServerDriver::GetInterfaceVersions()
{
    return ms_interfaces;
}

void CServerDriver::RunFrame()
{
    // Try to launch dashboard app every 5 seconds
    if(!m_dashboardLaunched)
    {
        if(GetTickCount64() - m_dashboardLaunchTick > 5000U)
        {
            std::string l_path(g_modulePath);
            l_path.erase(l_path.begin() + l_path.rfind('\\'), l_path.end());

            std::string l_appPath(l_path);
            l_appPath.append("\\kinect_dash.exe");
            STARTUPINFOA l_startupInfo = { 0 };
            PROCESS_INFORMATION l_processInfo = { 0 };
            l_startupInfo.cb = sizeof(STARTUPINFOA);
            m_dashboardLaunched = (CreateProcessA(l_appPath.c_str(), NULL, NULL, NULL, FALSE, 0, NULL, l_path.c_str(), &l_startupInfo, &l_processInfo) == TRUE);
            m_dashboardLaunchTick = GetTickCount64();
        }
    }

    if(m_kinectLock.try_lock())
    {
        const FrameData *l_frameData = m_kinectHandler->GetFrameData();
        if(m_frameHistoryCount == HI_Count)
        {
            if(m_frameHistory[HI_Last]->m_frameTime != l_frameData->m_frameTime)
            {
                // Copy 'Last' to 'Previous', current to 'Last'
                std::memcpy(m_frameHistory[HI_Previous], m_frameHistory[HI_Last], sizeof(FrameData));
                std::memcpy(m_frameHistory[HI_Last], l_frameData, sizeof(FrameData));
            }
        }
        else
        {
            FrameData *l_frameSnapshot = new FrameData();
            std::memcpy(l_frameSnapshot, l_frameData, sizeof(FrameData));
            m_frameHistory.push_back(l_frameSnapshot); // Frame time is irrelevant
            m_frameHistoryCount++;
        }
        m_kinectLock.unlock();
    }

    // Smooth history data
    if(m_frameHistoryCount == HI_Count)
    {
        const ULONGLONG l_diff = (GetTickCount64() - m_frameHistory[HI_Last]->m_tick);
        float l_smooth = static_cast<float>(l_diff) / 33.333333f;
        l_smooth = glm::clamp(l_smooth, 0.f, 1.f);

        switch(m_interpolation)
        {
            case CDriverConfig::FI_Quadratic:
                l_smooth = QuadraticEaseInOut(l_smooth);
                break;
            case CDriverConfig::FI_Cubic:
                l_smooth = CubicEaseInOut(l_smooth);
                break;
            case CDriverConfig::FI_Quartic:
                l_smooth = QuarticEaseInOut(l_smooth);
                break;
            case CDriverConfig::FI_Quintic:
                l_smooth = QuinticEaseInOut(l_smooth);
                break;
            case CDriverConfig::FI_Exponential:
                l_smooth = ExponentialEaseInOut(l_smooth);
                break;
            case CDriverConfig::FI_Sine:
                l_smooth = SineEaseInOut(l_smooth);
                break;
            case CDriverConfig::FI_Circular:
                l_smooth = CircularEaseInOut(l_smooth);
                break;
        }

        for(size_t i = 0U; i < NUI_SKELETON_POSITION_COUNT; i++)
        {
            if(m_trackers[i])
            {
                const JointData &l_jointA = m_frameHistory[HI_Previous]->m_joints[i];
                const JointData &l_jointB = m_frameHistory[HI_Last]->m_joints[i];

                const glm::vec3 l_jointPosA(-l_jointA.x, l_jointA.y, -l_jointA.z);
                const glm::vec3 l_jointPosB(-l_jointB.x, l_jointB.y, -l_jointB.z);

                const glm::quat l_jointRotA(l_jointA.rw, -l_jointA.rx, l_jointA.ry, -l_jointA.rz);
                const glm::quat l_jointRotB(l_jointB.rw, -l_jointB.rx, l_jointB.ry, -l_jointB.rz);

                const glm::vec3 l_linearPos = glm::mix(l_jointPosA, l_jointPosB, l_smooth);
                const glm::quat l_linearRot = glm::slerp(l_jointRotA, l_jointRotB, l_smooth);

                m_trackers[i]->SetPosition(l_linearPos.x, l_linearPos.y, l_linearPos.z);
                m_trackers[i]->SetRotation(l_linearRot.x, l_linearRot.y, l_linearRot.z, l_linearRot.w);
            }
        }
    }

    // Update devices
    m_kinectStation->RunFrame();
    for(auto l_tracker : m_trackers)
    {
        if(l_tracker) l_tracker->RunFrame();
    }
}

bool CServerDriver::ShouldBlockStandbyMode()
{
    return false;
}

void CServerDriver::EnterStandby()
{
}

void CServerDriver::LeaveStandby()
{
}

// CServerDriver
void CServerDriver::KinectProcess()
{
    const std::chrono::milliseconds l_threadDelay(33U);

    bool l_initialized = m_kinectHandler->Initialize();
    while(m_kinectActive)
    {
        if(l_initialized)
        {
            m_kinectLock.lock();
            m_kinectHandler->Update();
            m_kinectLock.unlock();
        }
        else l_initialized = m_kinectHandler->Initialize();

        std::this_thread::sleep_for(l_threadDelay);
    }

    if(l_initialized) m_kinectHandler->Terminate();
}

void CServerDriver::ProcessExternalMessage(const char *p_message)
{
    std::stringstream l_stream(p_message);
    std::string l_message;
    l_stream >> l_message;
    if(!l_message.empty() && !l_stream.fail())
    {
        switch(ReadEnumVector(l_message, g_messageNames))
        {
            case MessageIndex::MI_Calibration:
            {
                l_stream >> m_basePosition.x >> m_basePosition.y >> m_basePosition.z >> m_baseRotation.x >> m_baseRotation.y >> m_baseRotation.z >> m_baseRotation.w;
                if(!l_stream.fail())
                {
                    m_kinectStation->SetPosition(m_basePosition.x, m_basePosition.y, m_basePosition.z);
                    m_kinectStation->SetRotation(m_baseRotation.x, m_baseRotation.y, m_baseRotation.z, m_baseRotation.w);
                    for(auto l_tracker : m_trackers)
                    {
                        if(l_tracker)
                        {
                            l_tracker->SetOffsetPosition(m_basePosition.x, m_basePosition.y, m_basePosition.z);
                            l_tracker->SetOffsetRotation(m_baseRotation.x, m_baseRotation.y, m_baseRotation.z, m_baseRotation.w);
                        }
                    }
                }
            } break;
            case MessageIndex::MI_TrackingToggle:
            {
                m_trackingState = !m_trackingState;
                for(auto l_tracker : m_trackers)
                {
                    if(l_tracker) l_tracker->SetForcedConnected(m_trackingState);
                }
                m_kinectHandler->SetPaused(!m_trackingState);
            } break;
            case MessageIndex::MI_TrackerToggle:
            {
                size_t l_index = std::numeric_limits<size_t>::max();
                l_stream >> l_index;
                if(!l_stream.fail())
                {
                    if(l_index < NUI_SKELETON_POSITION_COUNT)
                    {
                        if(m_trackers[l_index]) m_trackers[l_index]->SetConnected(!m_trackers[l_index]->IsConnected());
                        else
                        {
                            m_trackers[l_index] = new CTrackerVive(l_index);
                            vr::VRServerDriverHost()->TrackedDeviceAdded(m_trackers[l_index]->GetSerial().c_str(), vr::TrackedDeviceClass_GenericTracker, m_trackers[l_index]);
                            m_trackers[l_index]->SetForcedConnected(m_trackingState);
                            m_trackers[l_index]->SetOffsetPosition(m_basePosition.x, m_basePosition.y, m_basePosition.z);
                            m_trackers[l_index]->SetOffsetRotation(m_baseRotation.x, m_baseRotation.y, m_baseRotation.z, m_baseRotation.w);
                        }
                    }
                }
            } break;
            case MessageIndex::MI_Interpolation:
            {
                l_stream >> m_interpolation;
                if(!l_stream.fail()) m_interpolation = glm::clamp<unsigned char>(m_interpolation, CDriverConfig::FI_Linear, CDriverConfig::FI_Circular);
            } break;
        }
    }
}
