#include "stdafx.h"

#include "CEmulatedDevice.h"

CEmulatedDevice::CEmulatedDevice()
{
    m_connected = false;
    m_forcedConnected = true;

    m_pose = { 0 };
    m_pose.poseTimeOffset = -0.011;
    m_pose.qWorldFromDriverRotation.w = 1.0;
    m_pose.qWorldFromDriverRotation.x = .0;
    m_pose.qWorldFromDriverRotation.y = .0;
    m_pose.qWorldFromDriverRotation.z = .0;
    m_pose.qDriverFromHeadRotation.w = 1.0;
    m_pose.qDriverFromHeadRotation.x = .0;
    m_pose.qDriverFromHeadRotation.y = .0;
    m_pose.qDriverFromHeadRotation.z = .0;
    m_pose.vecDriverFromHeadTranslation[0U] = .0;
    m_pose.vecDriverFromHeadTranslation[1U] = .0;
    m_pose.vecDriverFromHeadTranslation[2U] = .0;
    m_pose.poseIsValid = false;
    m_pose.willDriftInYaw = false;
    m_pose.shouldApplyHeadModel = false;
    m_pose.result = vr::TrackingResult_Uninitialized;
    m_pose.deviceIsConnected = false;

    m_propertyHandle = vr::k_ulInvalidPropertyContainer;
    m_trackedDevice = vr::k_unTrackedDeviceIndexInvalid;
}

CEmulatedDevice::~CEmulatedDevice()
{
}

// vr::ITrackedDeviceServerDriver
vr::EVRInitError CEmulatedDevice::Activate(uint32_t unObjectId)
{
    vr::EVRInitError l_error = vr::VRInitError_Driver_Failed;

    if(m_trackedDevice == vr::k_unTrackedDeviceIndexInvalid)
    {
        m_trackedDevice = unObjectId;
        m_propertyHandle = vr::VRProperties()->TrackedDeviceToPropertyContainer(m_trackedDevice);

        SetupProperties();

        m_connected = true;
        m_pose.deviceIsConnected = (m_connected && m_forcedConnected);
        m_pose.poseIsValid = true;
        m_pose.result = vr::TrackingResult_Running_OK;

        l_error = vr::VRInitError_None;
    }

    return l_error;
}

void CEmulatedDevice::Deactivate()
{
    m_trackedDevice = vr::k_unTrackedDeviceIndexInvalid;
}

void CEmulatedDevice::EnterStandby()
{
}

void* CEmulatedDevice::GetComponent(const char* pchComponentNameAndVersion)
{
    void *l_result = nullptr;
    if(!strcmp(pchComponentNameAndVersion, vr::ITrackedDeviceServerDriver_Version)) l_result = dynamic_cast<vr::ITrackedDeviceServerDriver*>(this);
    return l_result;
}

void CEmulatedDevice::DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize)
{
}

vr::DriverPose_t CEmulatedDevice::GetPose()
{
    return m_pose;
}

// CEmulatedDevice
const std::string& CEmulatedDevice::GetSerial() const
{
    return m_serial;
}

bool CEmulatedDevice::IsConnected() const
{
    return m_connected;
}

void CEmulatedDevice::SetConnected(bool p_state)
{
    m_connected = p_state;
    m_pose.deviceIsConnected = (m_connected && m_forcedConnected);
}

void CEmulatedDevice::SetForcedConnected(bool p_state)
{
    m_forcedConnected = p_state;
    m_pose.deviceIsConnected = (m_connected && m_forcedConnected);
}

void CEmulatedDevice::SetPosition(float p_x, float p_y, float p_z)
{
    m_pose.vecPosition[0U] = p_x;
    m_pose.vecPosition[1U] = p_y;
    m_pose.vecPosition[2U] = p_z;
}

void CEmulatedDevice::SetRotation(float p_x, float p_y, float p_z, float p_w)
{
    m_pose.qRotation.x = p_x;
    m_pose.qRotation.y = p_y;
    m_pose.qRotation.z = p_z;
    m_pose.qRotation.w = p_w;
}

void CEmulatedDevice::SetOffsetPosition(float p_x, float p_y, float p_z)
{
    m_pose.vecWorldFromDriverTranslation[0U] = p_x;
    m_pose.vecWorldFromDriverTranslation[1U] = p_y;
    m_pose.vecWorldFromDriverTranslation[2U] = p_z;
}

void CEmulatedDevice::SetOffsetRotation(float p_x, float p_y, float p_z, float p_w)
{
    m_pose.qWorldFromDriverRotation.x = p_x;
    m_pose.qWorldFromDriverRotation.y = p_y;
    m_pose.qWorldFromDriverRotation.z = p_z;
    m_pose.qWorldFromDriverRotation.w = p_w;
}

void CEmulatedDevice::SetupProperties()
{
}

void CEmulatedDevice::RunFrame()
{
    if(m_trackedDevice != vr::k_unTrackedDeviceIndexInvalid)
    {
        vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_trackedDevice, m_pose, sizeof(vr::DriverPose_t));
    }
}
