#include "stdafx.h"

#include "CKinectMonitor.h"
#include "CKinectConfig.h"

const char *g_notificationText = "Base calibration\nLeft touchpad: Rotate L/R/U/D\tRight touchpad: Move L/R, U/D and (hold trigger) F/B\tGrip: reset rotation/position\tMenu: Save calibration";
const std::chrono::milliseconds g_threadDelay(11U);

const float g_pi = glm::pi<float>();
const float g_pi2 = g_pi*2.f;
const float g_piHalf = g_pi * 0.5f;
const float g_piQuarter = g_pi * 0.25f;
const glm::vec3 g_axisX(1.f, 0.f, 0.f);
const glm::vec3 g_axisXN(-1.f, 0.f, 0.f);
const glm::vec3 g_axisY(0.f, 1.f, 0.f);
const glm::vec3 g_axisYN(0.f, -1.f, 0.f);
const glm::vec3 g_axisZ(0.f, 0.f, 1.f);
const glm::vec3 g_axisZN(0.f, 0.f, -1.f);

CKinectMonitor::CKinectMonitor()
{
    m_active = false;
    m_vrSystem = nullptr;
    m_overlay = vr::k_ulOverlayHandleInvalid;
    m_notification = 0U;
    m_kinectDevice = vr::k_unTrackedDeviceIndexInvalid;
    m_rightHand = vr::k_unTrackedDeviceIndexInvalid;
    m_leftHand = vr::k_unTrackedDeviceIndexInvalid;
    m_event = { 0 };
    m_kinectConfig = nullptr;
    m_basePosition = glm::vec3(0.f);
    m_baseRotation = glm::quat(1.f, 0.f, 0.f, 0.f);
    m_lastPressInfo = { vr::TrackedControllerRole_Invalid, CQ_Up, 0U, PS_None };
    m_triggerPressed = false;
}

CKinectMonitor::~CKinectMonitor()
{
}

bool CKinectMonitor::Initialize()
{
    if(!m_vrSystem)
    {
        vr::EVRInitError l_initError = vr::VRInitError_None;
        m_vrSystem = vr::VR_Init(&l_initError, vr::VRApplication_Overlay);
        if(l_initError == vr::VRInitError_None)
        {
            vr::VROverlay()->CreateOverlay("monitor.kinectV1.main", "KinectV1 Calibrator", &m_overlay);

            for(uint32_t i = 0U; i < vr::k_unMaxTrackedDeviceCount; i++)
            {
                if(m_vrSystem->IsTrackedDeviceConnected(i))
                {
                    if(m_vrSystem->GetUint64TrackedDeviceProperty(i, vr::Prop_VendorSpecific_Reserved_Start) == 0x4B696E6563745631) //"KinectV1"
                    {
                        m_kinectDevice = i;
                        break;
                    }
                }
            }
            m_leftHand = m_vrSystem->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_LeftHand);
            m_rightHand = m_vrSystem->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_RightHand);

            m_kinectConfig = new CKinectConfig("\\..\\..\\resources\\settings.xml");
            m_kinectConfig->Load();
            m_basePosition = m_kinectConfig->GetBasePosition();
            m_baseRotation = m_kinectConfig->GetBaseRotation();

            vr::VRNotifications()->CreateNotification(m_overlay, 0U, vr::EVRNotificationType_Persistent, g_notificationText, vr::EVRNotificationStyle_Application, nullptr, &m_notification);

            m_active = true;
        }
        else m_vrSystem = nullptr;
    }
    return (m_vrSystem != nullptr);
}
void CKinectMonitor::Terminate()
{
    if(m_vrSystem)
    {
        if(m_notification != 0U)
        {
            vr::VRNotifications()->RemoveNotification(m_notification);
            m_notification = 0U;
        }
        if(m_overlay != vr::k_ulOverlayHandleInvalid)
        {
            vr::VROverlay()->DestroyOverlay(m_overlay);
            m_overlay = vr::k_ulOverlayHandleInvalid;
        }

        vr::VR_Shutdown();
        m_vrSystem = nullptr;

        m_kinectDevice = vr::k_unTrackedDeviceIndexInvalid;
        m_rightHand = vr::k_unTrackedDeviceIndexInvalid;
        m_leftHand = vr::k_unTrackedDeviceIndexInvalid;

        delete m_kinectConfig;
        m_kinectConfig = nullptr;
    }
}

bool CKinectMonitor::DoPulse()
{
    while(m_vrSystem->PollNextEvent(&m_event, sizeof(vr::VREvent_t)))
    {
        switch(m_event.eventType)
        {
            case vr::VREvent_TrackedDeviceDeactivated:
            {
                if(m_leftHand == m_event.trackedDeviceIndex) m_leftHand = vr::k_unTrackedDeviceIndexInvalid;
                if(m_rightHand == m_event.trackedDeviceIndex) m_rightHand = vr::k_unTrackedDeviceIndexInvalid;
            } break;

            case vr::VREvent_ButtonPress:
            {
                switch(m_event.data.controller.button)
                {
                    case vr::k_EButton_SteamVR_Trigger:
                    {
                        if(m_event.trackedDeviceIndex == m_rightHand) m_triggerPressed = true;
                    } break;

                    case vr::k_EButton_Grip:
                    {
                        if(m_event.trackedDeviceIndex == m_leftHand)
                        {
                            m_baseRotation = glm::quat(1.f, 0.f, 0.f, 0.f);
                            m_kinectConfig->SetBaseRotation(m_baseRotation);
                            SendCalibration();
                        }
                        else if(m_event.trackedDeviceIndex == m_rightHand)
                        {
                            m_basePosition = glm::vec3(0.f);
                            m_kinectConfig->SetBasePosition(m_basePosition);
                            SendCalibration();
                        }
                    } break;

                    case vr::k_EButton_ApplicationMenu:
                        m_active = false;
                        break;
                }
            } break;

            case vr::VREvent_ButtonUnpress:
            {
                if(m_event.trackedDeviceIndex == m_rightHand)
                {
                    if(m_event.data.controller.button == vr::k_EButton_SteamVR_Trigger) m_triggerPressed = false;
                }
            } break;

            case vr::VREvent_ButtonTouch:
            {
                if((m_event.trackedDeviceIndex == m_leftHand) || (m_event.trackedDeviceIndex == m_rightHand))
                {
                    if(m_event.data.controller.button == vr::k_EButton_SteamVR_Touchpad)
                    {
                        vr::VRControllerState_t l_state;
                        m_vrSystem->GetControllerState(m_event.trackedDeviceIndex, &l_state, sizeof(vr::VRControllerState_t));

                        float l_theta = std::atan2f(l_state.rAxis[0U].y, l_state.rAxis[0U].x);
                        if(l_theta < 0.f) l_theta = g_pi2 + l_theta;
                        l_theta += g_piQuarter;
                        l_theta = std::fmod(l_theta, g_pi2);
                        m_lastPressInfo.m_quadrant = static_cast<unsigned char>(std::floorf((l_theta - std::fmod(l_theta, g_piHalf)) / g_piHalf));

                        m_lastPressInfo.m_hand = (m_event.trackedDeviceIndex == m_leftHand) ? vr::TrackedControllerRole_LeftHand : vr::TrackedControllerRole_RightHand;
                        m_lastPressInfo.m_tick = GetTickCount();
                        m_lastPressInfo.m_state = PS_Single;
                    }
                }
            } break;

            case vr::VREvent_ButtonUntouch:
            {
                if((m_event.trackedDeviceIndex == m_leftHand) || (m_event.trackedDeviceIndex == m_rightHand))
                {
                    if(m_event.data.controller.button == vr::k_EButton_SteamVR_Touchpad)
                    {
                        if(m_lastPressInfo.m_hand == ((m_event.trackedDeviceIndex == m_leftHand) ? vr::TrackedControllerRole_LeftHand : vr::TrackedControllerRole_RightHand))
                        {
                            m_lastPressInfo.m_hand = vr::TrackedControllerRole_Invalid;
                            m_lastPressInfo.m_tick = 0U;
                            m_lastPressInfo.m_state = PS_None;
                        }
                    }
                }
            } break;

            case vr::VREvent_Quit:
                m_active = false;
                break;
        }
    }

    if(m_lastPressInfo.m_hand != vr::TrackedControllerRole_Invalid)
    {
        bool l_update = false;

        switch(m_lastPressInfo.m_state)
        {
            case PS_Single:
            {
                m_lastPressInfo.m_state = PS_Wait;
                m_lastPressInfo.m_tick = GetTickCount64();
                l_update = true;
            } break;
            case PS_Wait:
            {
                ULONGLONG l_tick = GetTickCount64();
                if(l_tick - m_lastPressInfo.m_tick > 500U) // If hold 500ms
                {
                    m_lastPressInfo.m_state = PS_Continuous;
                    m_lastPressInfo.m_tick = l_tick;
                    l_update = true;
                }
            } break;
            case PS_Continuous:
            {
                ULONGLONG l_tick = GetTickCount64();
                if(l_tick - m_lastPressInfo.m_tick > 20U) // Step every 20ms
                {
                    m_lastPressInfo.m_tick = l_tick;
                    l_update = true;
                }
            } break;
        }

        if(l_update)
        {
            switch(m_lastPressInfo.m_hand)
            {
                case vr::TrackedControllerRole_LeftHand:
                {
                    switch(m_lastPressInfo.m_quadrant)
                    {
                        case CQ_Right:
                            m_baseRotation = glm::rotate(m_baseRotation, -g_pi / 180.f, g_axisY);
                            break;
                        case CQ_Left:
                            m_baseRotation = glm::rotate(m_baseRotation, g_pi / 180.f, g_axisY);
                            break;
                        case CQ_Up:
                            m_baseRotation = glm::rotate(m_baseRotation, g_pi / 180.f, g_axisX);
                            break;
                        case CQ_Down:
                            m_baseRotation = glm::rotate(m_baseRotation, -g_pi / 180.f, g_axisX);
                            break;
                    }
                } break;
                case vr::TrackedControllerRole_RightHand:
                {
                    switch(m_lastPressInfo.m_quadrant)
                    {
                        case CQ_Right:
                            m_basePosition += (m_baseRotation*g_axisX) * 0.01f;
                            break;
                        case CQ_Left:
                            m_basePosition += (m_baseRotation*g_axisXN)* 0.01f;
                            break;
                        case CQ_Up:
                            m_basePosition += (m_baseRotation*(m_triggerPressed ? g_axisZN : g_axisY)) * 0.01f;
                            break;
                        case CQ_Down:
                            m_basePosition += (m_baseRotation*(m_triggerPressed ? g_axisZ : g_axisYN)) * 0.01f;
                            break;
                    }
                }
            }

            m_kinectConfig->SetBasePosition(m_basePosition);
            m_kinectConfig->SetBaseRotation(m_baseRotation);
            SendCalibration();
        }
    }

    if(m_leftHand == vr::k_unTrackedDeviceIndexInvalid) m_leftHand = m_vrSystem->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_LeftHand);
    if(m_rightHand == vr::k_unTrackedDeviceIndexInvalid) m_rightHand = m_vrSystem->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_RightHand);

    std::this_thread::sleep_for(g_threadDelay);
    if(!m_active) m_kinectConfig->Save();
    return m_active;
}

void CKinectMonitor::SendCalibration()
{
    if(m_kinectDevice != vr::k_unTrackedDeviceIndexInvalid)
    {
        std::string l_data("calibrate");
        for(int i = 0; i < 3; i++)
        {
            l_data.push_back(' ');
            l_data.append(std::to_string(m_basePosition[i]));
        }
        for(int i = 0; i < 4; i++)
        {
            l_data.push_back(' ');
            l_data.append(std::to_string(m_baseRotation[i]));
        }

        char l_response[32U];
        vr::VRDebug()->DriverDebugRequest(m_kinectDevice, l_data.c_str(), l_response, 32U);
    }
}
