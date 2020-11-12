#include "stdafx.h"

#include "CKinectStation.h"
#include "CServerDriver.h"

CKinectStation::CKinectStation(CServerDriver *f_server)
{
    m_serverDriver = f_server;
    m_serial.assign("KS-10");
}

CKinectStation::~CKinectStation()
{
}

void CKinectStation::SetupProperties()
{
    vr::VRProperties()->SetStringProperty(m_propertyHandle, vr::Prop_TrackingSystemName_String, "kinect");
    vr::VRProperties()->SetStringProperty(m_propertyHandle, vr::Prop_SerialNumber_String, m_serial.c_str());
    vr::VRProperties()->SetStringProperty(m_propertyHandle, vr::Prop_ModelNumber_String, "Kinect Sensor 1.0");
    vr::VRProperties()->SetStringProperty(m_propertyHandle, vr::Prop_ManufacturerName_String, "Microsoft");
    vr::VRProperties()->SetStringProperty(m_propertyHandle, vr::Prop_ModeLabel_String, "K");

    vr::VRProperties()->SetInt32Property(m_propertyHandle, vr::Prop_DeviceClass_Int32, vr::TrackedDeviceClass_TrackingReference);
    vr::VRProperties()->SetBoolProperty(m_propertyHandle, vr::Prop_IsOnDesktop_Bool, false);
    vr::VRProperties()->SetBoolProperty(m_propertyHandle, vr::Prop_NeverTracked_Bool, false);
    vr::VRProperties()->SetBoolProperty(m_propertyHandle, vr::Prop_WillDriftInYaw_Bool, false);
    vr::VRProperties()->SetBoolProperty(m_propertyHandle, vr::Prop_CanWirelessIdentify_Bool, false);

    vr::VRProperties()->SetFloatProperty(m_propertyHandle, vr::Prop_FieldOfViewLeftDegrees_Float, 62.f);
    vr::VRProperties()->SetFloatProperty(m_propertyHandle, vr::Prop_FieldOfViewRightDegrees_Float, 62.f);
    vr::VRProperties()->SetFloatProperty(m_propertyHandle, vr::Prop_FieldOfViewTopDegrees_Float, 48.3f);
    vr::VRProperties()->SetFloatProperty(m_propertyHandle, vr::Prop_FieldOfViewBottomDegrees_Float, 48.3f);
    vr::VRProperties()->SetFloatProperty(m_propertyHandle, vr::Prop_TrackingRangeMinimumMeters_Float, 1.2f);
    vr::VRProperties()->SetFloatProperty(m_propertyHandle, vr::Prop_TrackingRangeMaximumMeters_Float, 3.5f);

    vr::VRProperties()->SetStringProperty(m_propertyHandle, vr::Prop_ResourceRoot_String, "kinectV1");
    vr::VRProperties()->SetStringProperty(m_propertyHandle, vr::Prop_RenderModelName_String, "{kinectV1}kinect_sensor_1_0");

    vr::VRProperties()->SetStringProperty(m_propertyHandle, vr::Prop_NamedIconPathDeviceOff_String, "{kinectV1}/icons/base_status_off.png");
    vr::VRProperties()->SetStringProperty(m_propertyHandle, vr::Prop_NamedIconPathDeviceSearching_String, "{kinectV1}/icons/base_status_searching.gif");
    vr::VRProperties()->SetStringProperty(m_propertyHandle, vr::Prop_NamedIconPathDeviceSearchingAlert_String, "{kinectV1}/icons/base_status_searching_alert.gif");
    vr::VRProperties()->SetStringProperty(m_propertyHandle, vr::Prop_NamedIconPathDeviceReady_String, "{kinectV1}/icons/base_status_ready.png");
    vr::VRProperties()->SetStringProperty(m_propertyHandle, vr::Prop_NamedIconPathDeviceReadyAlert_String, "{kinectV1}/icons/base_status_ready_alert.png");
    vr::VRProperties()->SetStringProperty(m_propertyHandle, vr::Prop_NamedIconPathDeviceNotReady_String, "{kinectV1}/icons/base_status_error.png");
    vr::VRProperties()->SetStringProperty(m_propertyHandle, vr::Prop_NamedIconPathDeviceStandby_String, "{kinectV1}/icons/base_status_standby.png");

    vr::VRProperties()->SetBoolProperty(m_propertyHandle, vr::Prop_HasDisplayComponent_Bool, false);
    vr::VRProperties()->SetBoolProperty(m_propertyHandle, vr::Prop_HasCameraComponent_Bool, false);
    vr::VRProperties()->SetBoolProperty(m_propertyHandle, vr::Prop_HasDriverDirectModeComponent_Bool, false);
    vr::VRProperties()->SetBoolProperty(m_propertyHandle, vr::Prop_HasVirtualDisplayComponent_Bool, false);

    vr::VRProperties()->SetUint64Property(m_propertyHandle, vr::Prop_VendorSpecific_Reserved_Start, 0x4B696E6563745631); // "KinectV1"
}

void CKinectStation::DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize)
{
    m_serverDriver->ProcessExternalMessage(pchRequest);
}
