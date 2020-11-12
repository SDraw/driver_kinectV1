#pragma once
#include "CEmulatedDevice.h"

class CServerDriver;

class CKinectStation final : public CEmulatedDevice
{
    CServerDriver *m_serverDriver;

    CKinectStation(const CKinectStation &that) = delete;
    CKinectStation& operator=(const CKinectStation &that) = delete;

    void SetupProperties() override;

    // vr::ITrackedDeviceServerDriver
    void DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize) override;
public:
    explicit CKinectStation(CServerDriver *f_server);
    ~CKinectStation();
};
