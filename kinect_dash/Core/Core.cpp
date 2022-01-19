#include "stdafx.h"

#include "Core/Core.h"
#include "Managers/ConfigManager.h"
#include "Managers/SfmlManager.h"
#include "Managers/GuiManager.h"
#include "Managers/VRManager.h"

const std::chrono::milliseconds g_threadSleep(33U); // ~30 fps lock

Core::Core()
{
    m_valid = false;
    m_active = false;

    m_configManager = nullptr;
    m_vrManager = nullptr;
    m_sfmlManager = nullptr;
    m_guiManager = nullptr;
}
Core::~Core()
{
}

bool Core::Intitialize()
{
    if(!m_valid)
    {
        m_configManager = new ConfigManager();
        m_configManager->Load();

        try
        {
            m_vrManager = new VRManager(this);
            m_sfmlManager = new SfmlManager(this);
            m_guiManager = new GuiManager(this);

            m_vrManager->SetOverlayTexture(m_guiManager->GetRenderTargetTextureName());

            m_valid = true;
            m_active = true;
        }
        catch(std::exception &l_exception)
        {
            MessageBoxA(NULL, l_exception.what(), "KinectV1 Dashboard", MB_OK | MB_ICONEXCLAMATION);
        }
    }
    return m_valid;
}
void Core::Terminate()
{
    if(m_valid)
    {
        delete m_guiManager;
        m_guiManager = nullptr;

        delete m_sfmlManager;
        m_sfmlManager = nullptr;

        delete m_vrManager;
        m_vrManager = nullptr;

        m_configManager->Save();
        delete m_configManager;
        m_configManager = nullptr;

        m_valid = false;
    }
}

bool Core::DoPulse()
{
    if(m_active)
    {
        m_active = (m_active && m_vrManager->DoPulse());
        m_active = (m_active && m_sfmlManager->DoPulse());

        std::this_thread::sleep_for(g_threadSleep);
    }
    return m_active;
}

ConfigManager* Core::GetConfigManager() const
{
    return m_configManager;
}
VRManager* Core::GetVRManager() const
{
    return m_vrManager;
}
SfmlManager* Core::GetSfmlManager() const
{
    return m_sfmlManager;
}
GuiManager* Core::GetGuiManager() const
{
    return m_guiManager;
}
