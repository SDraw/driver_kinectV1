#pragma once

class CEmulatedDevice;
class CKinectHandler;
struct FrameData;

class CServerDriver final : public vr::IServerTrackedDeviceProvider
{
    static const char* const ms_interfaces[];

    CKinectHandler *m_kinectHandler;
    std::thread *m_kinectThread;
    std::mutex m_kinectLock;
    std::atomic<bool> m_kinectActive;
    std::vector<FrameData*> m_frameHistory;
    size_t m_frameHistoryCount;

    CEmulatedDevice *m_kinectStation;
    CEmulatedDevice *m_trackers[NUI_SKELETON_POSITION_COUNT];

    glm::vec3 m_basePosition;
    glm::quat m_baseRotation;
    bool m_trackingState;
    unsigned char m_interpolation;

    ULONGLONG m_dashboardLaunchTick;
    bool m_dashboardLaunched;

    CServerDriver(const CServerDriver &that) = delete;
    CServerDriver& operator=(const CServerDriver &that) = delete;

    void KinectProcess();

    // vr::IServerTrackedDeviceProvider
    vr::EVRInitError Init(vr::IVRDriverContext *pDriverContext);
    void Cleanup();
    const char* const* GetInterfaceVersions();
    void RunFrame();
    bool ShouldBlockStandbyMode();
    void EnterStandby();
    void LeaveStandby();
public:
    CServerDriver();
    ~CServerDriver();

    void ProcessExternalMessage(const char *p_message);
};
