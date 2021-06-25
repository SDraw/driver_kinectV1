#include "stdafx.h"

#include "Core/Core.h"
#include "Managers/VRManager.h"
#include "Managers/GuiManager.h"
#include "Managers/CalibrationHelper.h"

const char *g_notificationText = "Base calibration\nLeft controller: Rotate L/R/U/D\tRight controller: Move L/R, U/D and (hold trigger) F/B\tGrip: reset rotation/position";

VRManager::VRManager(Core *f_core)
{
    m_core = f_core;

    vr::EVRInitError l_initError;
    m_vrSystem = vr::VR_Init(&l_initError, vr::VRApplication_Overlay);
    if(l_initError != vr::VRInitError_None)
    {
        std::string l_errorString("Unable to initialize OpenVR: ");
        l_errorString.append(vr::VR_GetVRInitErrorAsEnglishDescription(l_initError));
        throw std::runtime_error(l_errorString);
    }

#ifndef DASHBOARD_DESKTOP
    // Create overlays
    vr::VROverlay()->CreateDashboardOverlay("kinect_dash.dashboard", "KinectV1 Dashboard", &m_dashboardOverlay, &m_dashboardOverlayThumbnail);
    if((m_dashboardOverlay != vr::k_ulOverlayHandleInvalid) && (m_dashboardOverlayThumbnail != vr::k_ulOverlayHandleInvalid))
    {
        std::string l_fullPath(MAX_PATH, '\0');
        l_fullPath.resize(GetCurrentDirectoryA(MAX_PATH, &l_fullPath[0U]));
        l_fullPath.append("\\..\\..\\resources\\icons\\base.png");
        vr::VROverlay()->SetOverlayFromFile(m_dashboardOverlayThumbnail, l_fullPath.c_str());

        vr::HmdVector2_t l_mouseScale{ { 1024.f, 512.f } };
        vr::VROverlay()->SetOverlayInputMethod(m_dashboardOverlay, vr::VROverlayInputMethod_Mouse);
        vr::VROverlay()->SetOverlayMouseScale(m_dashboardOverlay, &l_mouseScale);
        vr::VROverlay()->SetOverlayWidthInMeters(m_dashboardOverlay, 3.f);

        m_dashboardTexture.eColorSpace = vr::ColorSpace_Gamma;
        m_dashboardTexture.eType = vr::TextureType_OpenGL;
        m_dashboardTexture.handle = nullptr;
    }
#endif

    vr::VROverlay()->CreateOverlay("kinect_dash.notify", "KinectV1 Calibration", &m_notificationOverlay);
    m_notification = 0;

    m_kinectDevice = vr::k_unTrackedDeviceIndexInvalid;
    m_leftHand = vr::k_unTrackedDeviceIndexInvalid;
    m_rightHand = vr::k_unTrackedDeviceIndexInvalid;
    m_event = { 0 };

    for(uint32_t i = 0U; i < vr::k_unMaxTrackedDeviceCount; i++)
    {
        if(m_vrSystem->IsTrackedDeviceConnected(i))
        {
            if(m_vrSystem->GetUint64TrackedDeviceProperty(i, vr::Prop_VendorSpecific_Reserved_Start) == 0x4B696E6563745631)
            {
                m_kinectDevice = i;
                break;
            }
        }
    }
    m_leftHand = m_vrSystem->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_LeftHand);
    m_rightHand = m_vrSystem->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_RightHand);

    m_calibrationHelper = new CalibrationHelper(this);

    m_active = true;
}
VRManager::~VRManager()
{
    if(m_notificationOverlay != vr::k_ulOverlayHandleInvalid) vr::VROverlay()->DestroyOverlay(m_notificationOverlay);
    if(m_notification != 0) vr::VRNotifications()->RemoveNotification(m_notification);
#ifndef DASHBOARD_DESKTOP
    if(m_dashboardOverlay != vr::k_ulOverlayHandleInvalid) vr::VROverlay()->DestroyOverlay(m_dashboardOverlay);
    if(m_dashboardOverlayThumbnail != vr::k_ulOverlayHandleInvalid) vr::VROverlay()->DestroyOverlay(m_dashboardOverlayThumbnail);
#endif
    vr::VR_Shutdown();
}

Core* VRManager::GetCore() const
{
    return m_core;
}

bool VRManager::DoPulse()
{
    while(m_vrSystem->PollNextEvent(&m_event, sizeof(vr::VREvent_t)))
    {
        switch(m_event.eventType)
        {
            case vr::VREvent_ButtonPress: case vr::VREvent_ButtonUnpress:
            {
                if(m_calibrationHelper->IsCalibrationActive())
                {
                    switch(m_event.data.controller.button)
                    {
                        case vr::k_EButton_SteamVR_Trigger:
                        {
                            if(m_event.trackedDeviceIndex == m_leftHand) m_calibrationHelper->RecieveControllerTrigger(true, m_event.eventType == vr::VREvent_ButtonPress);
                            if(m_event.trackedDeviceIndex == m_rightHand) m_calibrationHelper->RecieveControllerTrigger(false, m_event.eventType == vr::VREvent_ButtonPress);
                        } break;

                        case vr::k_EButton_ApplicationMenu:
                        {
                            if(m_event.trackedDeviceIndex == m_rightHand)
                            {
                                vr::VRNotifications()->RemoveNotification(m_notification);
                                m_notification = 0;
                                m_calibrationHelper->StopCalibration();
                            }
                        } break;

                        case vr::k_EButton_Grip:
                        {
                            if(m_event.trackedDeviceIndex == m_leftHand) m_calibrationHelper->ReciveControllerGrip(true, m_event.eventType == vr::VREvent_ButtonPress);
                            if(m_event.trackedDeviceIndex == m_rightHand) m_calibrationHelper->ReciveControllerGrip(false, m_event.eventType == vr::VREvent_ButtonPress);
                        } break;
                    }
                }
            } break;

            case vr::VREvent_TrackedDeviceDeactivated:
            {
                if(m_leftHand == m_event.trackedDeviceIndex)
                {
                    m_leftHand = vr::k_unTrackedDeviceIndexInvalid;
                    if(m_calibrationHelper->IsCalibrationActive()) m_calibrationHelper->RecieveControllerDisconnect(true);
                }
                if(m_rightHand == m_event.trackedDeviceIndex)
                {
                    m_rightHand = vr::k_unTrackedDeviceIndexInvalid;
                    if(m_calibrationHelper->IsCalibrationActive()) m_calibrationHelper->RecieveControllerDisconnect(false);
                }
            } break;

            case vr::VREvent_Quit:
                m_active = false;
                break;
        }
    }

#ifndef DASHBOARD_DESKTOP
    while(vr::VROverlay()->PollNextOverlayEvent(m_dashboardOverlay, &m_event, sizeof(vr::VREvent_t)))
    {
        switch(m_event.eventType)
        {
            case vr::VREvent_MouseMove:
            {
                m_core->GetGuiManager()->ReceiveMouseMove(m_event.data.mouse.x, m_event.data.mouse.y);
            } break;
            case vr::VREvent_MouseButtonDown: case vr::VREvent_MouseButtonUp:
            {
                m_core->GetGuiManager()->ReceiveMouseClick(m_event.data.mouse.button == vr::VRMouseButton_Left, m_event.eventType == vr::VREvent_MouseButtonDown);
            } break;
        }
    }
#endif

    if(m_leftHand == vr::k_unTrackedDeviceIndexInvalid) m_leftHand = m_vrSystem->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_LeftHand);
    if(m_rightHand == vr::k_unTrackedDeviceIndexInvalid) m_rightHand = m_vrSystem->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_RightHand);

#ifndef DASHBOARD_DESKTOP
    if(IsOverlayVisible()) vr::VROverlay()->SetOverlayTexture(m_dashboardOverlay, &m_dashboardTexture);
#endif

    // Calibration helper update
    if(m_calibrationHelper->IsCalibrationActive())
    {
        if(m_leftHand != vr::k_unTrackedDeviceIndexInvalid)
        {
            vr::VRControllerState_t l_state;
            m_vrSystem->GetControllerState(m_leftHand, &l_state, sizeof(vr::VRControllerState_t));

            glm::vec2 l_axis(l_state.rAxis[0U].x, l_state.rAxis[0U].y);
            m_calibrationHelper->RecieveControllerAxis(true, l_axis);
        }
        if(m_rightHand != vr::k_unTrackedDeviceIndexInvalid)
        {
            vr::VRControllerState_t l_state;
            m_vrSystem->GetControllerState(m_rightHand, &l_state, sizeof(vr::VRControllerState_t));

            glm::vec2 l_axis(l_state.rAxis[0U].x, l_state.rAxis[0U].y);
            m_calibrationHelper->RecieveControllerAxis(false, l_axis);
        }
    }
    m_calibrationHelper->DoPulse();

    return m_active;
}

bool VRManager::IsKinectDevicePresent() const
{
    return (m_kinectDevice != vr::k_unTrackedDeviceIndexInvalid);
}

#ifndef DASHBOARD_DESKTOP
bool VRManager::IsOverlayVisible() const
{
    bool l_result = false;
    if(m_vrSystem)
    {
        if(m_dashboardOverlay != vr::k_ulOverlayHandleInvalid) l_result = vr::VROverlay()->IsOverlayVisible(m_dashboardOverlay);
    }
    return l_result;
}

void VRManager::SetOverlayTexture(unsigned int f_name)
{
    if(m_dashboardOverlay != vr::k_ulOverlayHandleInvalid) m_dashboardTexture.handle = reinterpret_cast<void*>(static_cast<uintptr_t>(f_name));;
}
#endif

void VRManager::SendTrackerToggle(size_t f_index)
{
    if(m_kinectDevice != vr::k_unTrackedDeviceIndexInvalid)
    {
        std::string l_message("tracker_toggle ");
        l_message.append(std::to_string(f_index));
        char l_response[32U] = { 0 };
        vr::VRDebug()->DriverDebugRequest(m_kinectDevice, l_message.c_str(), l_response, 32U);
    }
}

void VRManager::SendTrackingToggle()
{
    if(m_kinectDevice != vr::k_unTrackedDeviceIndexInvalid)
    {
        char l_response[32U] = { 0 };
        vr::VRDebug()->DriverDebugRequest(m_kinectDevice, "tracking_toggle", l_response, 32U);
    }
}

void VRManager::SendInterpolationChange(size_t f_type)
{
    if(m_kinectDevice != vr::k_unTrackedDeviceIndexInvalid)
    {
        std::string l_message("interpolation ");
        l_message.append(std::to_string(f_type));
        char l_response[32U] = { 0 };
        vr::VRDebug()->DriverDebugRequest(m_kinectDevice, l_message.c_str(), l_response, 32U);
    }
}

void VRManager::SendCalibration(const glm::vec3 &f_pos, const glm::quat &f_rot)
{
    if(m_kinectDevice != vr::k_unTrackedDeviceIndexInvalid)
    {
        std::string l_message("calibration ");
        for(int i = 0; i < 3; i++)
        {
            l_message.append(std::to_string(f_pos[i]));
            l_message.push_back(' ');
        }
        for(int i = 0; i < 4; i++)
        {
            l_message.append(std::to_string(f_rot[i]));
            if(i < 3) l_message.push_back(' ');
        }

        char l_response[32U] = { 0 };
        vr::VRDebug()->DriverDebugRequest(m_kinectDevice, l_message.c_str(), l_response, 32U);
    }
}

void VRManager::LaunchCalibration()
{
    if(!m_calibrationHelper->IsCalibrationActive())
    {
        vr::VRNotifications()->CreateNotification(m_notificationOverlay, 0U, vr::EVRNotificationType_Persistent, g_notificationText, vr::EVRNotificationStyle_Application, nullptr, &m_notification);

        m_calibrationHelper->StartCalibration();
    }
}