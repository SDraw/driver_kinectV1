#pragma once

class CEmulatedDevice : public vr::ITrackedDeviceServerDriver
{
    vr::DriverPose_t m_pose;
    bool m_connected;
    bool m_forcedConnected;

    CEmulatedDevice(const CEmulatedDevice &that) = delete;
    CEmulatedDevice& operator=(const CEmulatedDevice &that) = delete;

    // vr::ITrackedDeviceServerDriver
    vr::EVRInitError Activate(uint32_t unObjectId);
    void Deactivate();
    void EnterStandby();
    void* GetComponent(const char* pchComponentNameAndVersion);
    void DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize);
    vr::DriverPose_t GetPose();
public:
    CEmulatedDevice();
    virtual ~CEmulatedDevice();

    const std::string& GetSerial() const;
    const size_t GetIndex() const;

    bool IsConnected() const;
    void SetConnected(bool f_state);
    void SetForcedConnected(bool f_state);

    void SetPosition(float f_x, float f_y, float f_z);
    void SetRotation(float f_x, float f_y, float f_z, float f_w);

    void SetOffsetPosition(float f_x, float f_y, float f_z);
    void SetOffsetRotation(float f_x, float f_y, float f_z, float f_w);

    void RunFrame();
protected:
    vr::PropertyContainerHandle_t m_propertyHandle;
    uint32_t m_trackedDevice;

    std::string m_serial;

    virtual void SetupProperties();
};
