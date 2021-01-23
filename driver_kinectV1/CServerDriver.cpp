#include "stdafx.h"

#include "CServerDriver.h"
#include "CEmulatedDevice.h"
#include "CTrackerVive.h"
#include "CKinectStation.h"
#include "CKinectHandler.h"

#include "CDriverConfig.h"
#include "MathUtils.h"
#include "Utils.h"

const std::vector<std::string> g_messageNames
{
    "calibrate", "switch"
};
enum MessageIndex : size_t
{
    MI_Calibrate = 0U,
    MI_Switch
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
    m_hotkeyState = false;
}

CServerDriver::~CServerDriver()
{
}

// vr::IServerTrackedDeviceProvider
vr::EVRInitError CServerDriver::Init(vr::IVRDriverContext *pDriverContext)
{
    VR_INIT_SERVER_DRIVER_CONTEXT(pDriverContext);
    CDriverConfig::Load();

    m_kinectHandler = new CKinectHandler(CDriverConfig::GetBoneIndexes());
    m_kinectActive = true;
    m_kinectThread = new std::thread(&CServerDriver::KinectProcess, this);

    // Add trackers
    for(auto l_index : CDriverConfig::GetBoneIndexes())
    {
        CEmulatedDevice *l_tracker = new CTrackerVive(l_index);
        m_trackers.push_back(l_tracker);
        vr::VRServerDriverHost()->TrackedDeviceAdded(l_tracker->GetSerial().c_str(), vr::TrackedDeviceClass_GenericTracker, l_tracker);
    }

    // Add fake station as command relay with Kinect model
    m_kinectStation = new CKinectStation(this);
    vr::VRServerDriverHost()->TrackedDeviceAdded(m_kinectStation->GetSerial().c_str(), vr::TrackedDeviceClass_TrackingReference, m_kinectStation);

    // Coordinates offset
    const glm::vec3 &l_position = CDriverConfig::GetBasePosition();
    const glm::quat &l_rotation = CDriverConfig::GetBaseRotation();
    m_kinectStation->SetPosition(l_position.x, l_position.y, l_position.z);
    m_kinectStation->SetRotation(l_rotation.x, l_rotation.y, l_rotation.z, l_rotation.w);
    for(auto l_tracker : m_trackers)
    {
        l_tracker->SetOffsetPosition(l_position.x, l_position.y, l_position.z);
        l_tracker->SetOffsetRotation(l_rotation.x, l_rotation.y, l_rotation.z, l_rotation.w);
    }

    return vr::VRInitError_None;
}

void CServerDriver::Cleanup()
{
    for(auto l_tracker : m_trackers) delete l_tracker;
    m_trackers.clear();

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

        switch(CDriverConfig::GetInterpolation())
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

        for(auto l_tracker : m_trackers)
        {
            const size_t l_index = l_tracker->GetIndex();
            const JointData &l_jointA = m_frameHistory[HI_Previous]->m_joints[l_index];
            const JointData &l_jointB = m_frameHistory[HI_Last]->m_joints[l_index];

            const glm::vec3 l_jointPosA(-l_jointA.x, l_jointA.y, -l_jointA.z);
            const glm::vec3 l_jointPosB(-l_jointB.x, l_jointB.y, -l_jointB.z);

            const glm::quat l_jointRotA(l_jointA.rw, -l_jointA.rx, l_jointA.ry, -l_jointA.rz);
            const glm::quat l_jointRotB(l_jointB.rw, -l_jointB.rx, l_jointB.ry, -l_jointB.rz);

            const glm::vec3 l_linearPos = glm::mix(l_jointPosA, l_jointPosB, l_smooth);
            const glm::quat l_linearRot = glm::slerp(l_jointRotA, l_jointRotB, l_smooth);

            l_tracker->SetPosition(l_linearPos.x, l_linearPos.y, l_linearPos.z);
            l_tracker->SetRotation(l_linearRot.x, l_linearRot.y, l_linearRot.z, l_linearRot.w);
        }
    }

    const bool l_hotkeyState = ((GetAsyncKeyState(VK_CONTROL) & 0x8000) && (GetAsyncKeyState(0x54) & 0x8000)); // Ctrl+T
    if(m_hotkeyState != l_hotkeyState)
    {
        m_hotkeyState = l_hotkeyState;
        if(m_hotkeyState)
        {
            // Switch tracking
            for(auto l_tracker : m_trackers)
            {
                const bool l_connected = l_tracker->IsConnected();
                l_tracker->SetConnected(!l_connected);
            }
            m_kinectHandler->SetPaused(!m_kinectHandler->IsPaused());
        }
    }

    // Update devices
    m_kinectStation->RunFrame();
    for(auto l_tracker : m_trackers) l_tracker->RunFrame();
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
    
    // Old Kinect fails initialization if not connected
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

void CServerDriver::ProcessExternalMessage(const char *f_message)
{
    // [ Messages ]
    // "calibrate x y z rx ry rz rw" - change base offset
    // "switch" - switch devices and tracking
    std::stringstream l_stream(f_message);

    std::string l_message;
    l_stream >> l_message;
    if(!l_message.empty() && !l_stream.fail())
    {
        switch(ReadEnumVector(l_message, g_messageNames))
        {
            case MessageIndex::MI_Calibrate:
            {
                glm::vec3 l_position(0.f);
                glm::quat l_rotation(1.f, 0.f, 0.f, 0.f);
                l_stream >> l_position.x >> l_position.y >> l_position.z >> l_rotation.x >> l_rotation.y >> l_rotation.z >> l_rotation.w;

                m_kinectStation->SetPosition(l_position.x, l_position.y, l_position.z);
                m_kinectStation->SetRotation(l_rotation.x, l_rotation.y, l_rotation.z, l_rotation.w);
                for(auto l_tracker : m_trackers)
                {
                    l_tracker->SetOffsetPosition(l_position.x, l_position.y, l_position.z);
                    l_tracker->SetOffsetRotation(l_rotation.x, l_rotation.y, l_rotation.z, l_rotation.w);
                }
            } break;
            case MessageIndex::MI_Switch:
            {
                for(auto l_tracker : m_trackers)
                {
                    const bool l_connected = l_tracker->IsConnected();
                    l_tracker->SetConnected(!l_connected);
                }
                m_kinectHandler->SetPaused(!m_kinectHandler->IsPaused());
            } break;
        }
    }
}
