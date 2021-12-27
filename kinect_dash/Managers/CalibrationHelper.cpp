#include "stdafx.h"

#include "Managers/CalibrationHelper.h"

#include "Core/Core.h"
#include "Managers/ConfigManager.h"
#include "Managers/VRManager.h"

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

CalibrationHelper::CalibrationHelper(VRManager *p_vrManager)
{
    m_vrManager = p_vrManager;
    m_active = false;

    m_inputInfoLeft.m_quadrant = CQ_None;
    m_inputInfoLeft.m_trigger = false;

    m_inputInfoRight.m_quadrant = CQ_None;
    m_inputInfoRight.m_trigger = false;
}
CalibrationHelper::~CalibrationHelper()
{
}

void CalibrationHelper::StartCalibration()
{
    if(!m_active)
    {
        m_active = true;

        std::memcpy(&m_basePosition, &m_vrManager->GetCore()->GetConfigManager()->GetBasePosition(), sizeof(glm::vec3));
        std::memcpy(&m_baseRotation, &m_vrManager->GetCore()->GetConfigManager()->GetBaseRotation(), sizeof(glm::quat));
    }
}

void CalibrationHelper::StopCalibration()
{
    if(m_active)
    {
        m_active = false;

        m_vrManager->GetCore()->GetConfigManager()->SetBasePosition(m_basePosition);
        m_vrManager->GetCore()->GetConfigManager()->SetBaseRotation(m_baseRotation);

        m_inputInfoLeft.m_quadrant = CQ_None;
        m_inputInfoLeft.m_trigger = false;

        m_inputInfoRight.m_quadrant = CQ_None;
        m_inputInfoRight.m_trigger = false;
    }
}

bool CalibrationHelper::IsCalibrationActive() const
{
    return m_active;
}

void CalibrationHelper::RecieveControllerAxis(bool p_left, const glm::vec2 &p_axis)
{
    if(m_active)
    {
        InputInfo &l_inputInfo = (p_left ? m_inputInfoLeft : m_inputInfoRight);

        if(glm::length(p_axis) >= 0.5f)
        {
            float l_theta = std::atan2f(p_axis.y, p_axis.x);
            if(l_theta < 0.f) l_theta = g_pi2 + l_theta;
            l_theta += g_piQuarter;
            l_theta = std::fmod(l_theta, g_pi2);
            l_inputInfo.m_quadrant = static_cast<unsigned char>(std::floorf((l_theta - std::fmod(l_theta, g_piHalf)) / g_piHalf));
        }
        else l_inputInfo.m_quadrant = CQ_None;
    }
}
void CalibrationHelper::RecieveControllerTrigger(bool p_left, bool p_state)
{
    if(m_active)
    {
        InputInfo &l_inputInfo = (p_left ? m_inputInfoLeft : m_inputInfoRight);
        l_inputInfo.m_trigger = p_state;
    }
}
void CalibrationHelper::ReciveControllerGrip(bool p_left, bool p_state)
{
    if(m_active)
    {
        if(p_state)
        {
            if(p_left) m_baseRotation = glm::quat(1.f,0.f,0.f,0.f);
            else m_basePosition = glm::vec3(0.f);

            m_vrManager->SendCalibration(m_basePosition, m_baseRotation);
            m_vrManager->GetCore()->GetConfigManager()->SetBasePosition(m_basePosition);
            m_vrManager->GetCore()->GetConfigManager()->SetBaseRotation(m_baseRotation);
        }
    }
}
void CalibrationHelper::RecieveControllerDisconnect(bool p_left)
{
    if(m_active)
    {
        InputInfo &l_inputInfo = (p_left ? m_inputInfoLeft : m_inputInfoRight);
        l_inputInfo.m_quadrant = CQ_None;
        l_inputInfo.m_trigger = false;
    }
}

void CalibrationHelper::DoPulse()
{
    if(m_active)
    {
        if(m_inputInfoLeft.m_quadrant != CQ_None)
        {
            switch(m_inputInfoLeft.m_quadrant)
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
        }

        if(m_inputInfoRight.m_quadrant != CQ_None)
        {
            switch(m_inputInfoRight.m_quadrant)
            {
                case CQ_Right:
                    m_basePosition += (m_baseRotation*g_axisX) * 0.01f;
                    break;
                case CQ_Left:
                    m_basePosition += (m_baseRotation*g_axisXN)* 0.01f;
                    break;
                case CQ_Up:
                    m_basePosition += (m_baseRotation*(m_inputInfoRight.m_trigger ? g_axisZN : g_axisY)) * 0.01f;
                    break;
                case CQ_Down:
                    m_basePosition += (m_baseRotation*(m_inputInfoRight.m_trigger ? g_axisZ : g_axisYN)) * 0.01f;
                    break;
            }
        }

        if(m_inputInfoLeft.m_quadrant != CQ_None || m_inputInfoRight.m_quadrant != CQ_None)
        {
            m_vrManager->SendCalibration(m_basePosition, m_baseRotation);
            m_vrManager->GetCore()->GetConfigManager()->SetBasePosition(m_basePosition);
            m_vrManager->GetCore()->GetConfigManager()->SetBaseRotation(m_baseRotation);
        }
    }
}