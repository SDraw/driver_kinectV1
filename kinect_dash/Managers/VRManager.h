#pragma once

class Core;
class CalibrationHelper;

class VRManager final
{
    friend Core;
    Core *m_core;

    bool m_active;
    vr::IVRSystem *m_vrSystem;

    vr::VROverlayHandle_t m_notificationOverlay;
    vr::VRNotificationId m_notification;

#ifndef DASHBOARD_DESKTOP
    vr::VROverlayHandle_t m_dashboardOverlay;
    vr::VROverlayHandle_t m_dashboardOverlayThumbnail;
    vr::Texture_t m_dashboardTexture;
#endif

    vr::TrackedDeviceIndex_t m_kinectDevice;
    vr::TrackedDeviceIndex_t m_leftHand;
    vr::TrackedDeviceIndex_t m_rightHand;

    vr::VREvent_t m_event;

    CalibrationHelper *m_calibrationHelper;

    VRManager(const VRManager &that) = delete;
    VRManager& operator=(const VRManager &that) = delete;
public:
    Core* GetCore() const;

    bool IsKinectDevicePresent() const;
#ifndef DASHBOARD_DESKTOP
    bool IsOverlayVisible() const;
#endif

    void SendTrackerToggle(size_t f_index);
    void SendTrackingToggle();
    void SendInterpolationChange(size_t f_type);
    void SendCalibration(const glm::vec3 &f_pos, const glm::quat &f_rot);

    void LaunchCalibration();
protected:
    explicit VRManager(Core *f_core);
    ~VRManager();

#ifndef DASHBOARD_DESKTOP
    void SetOverlayTexture(unsigned int f_name);
#endif

    bool DoPulse();
};

