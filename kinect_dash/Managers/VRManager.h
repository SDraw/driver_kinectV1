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

    vr::VROverlayHandle_t m_dashboardOverlay;
    vr::VROverlayHandle_t m_dashboardOverlayThumbnail;
    vr::Texture_t m_dashboardTexture;

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
    bool IsOverlayVisible() const;

    void SendTrackerToggle(size_t p_index);
    void SendTrackingToggle();
    void SendInterpolationChange(size_t p_type);
    void SendCalibration(const glm::vec3 &p_pos, const glm::quat &p_rot);

    void LaunchCalibration();
protected:
    explicit VRManager(Core *p_core);
    ~VRManager();

    void SetOverlayTexture(unsigned int p_name);

    bool DoPulse();
};

