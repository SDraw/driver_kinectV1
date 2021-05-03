#pragma once

class SfmlManager;
class VRManager;
class ConfigManager;
class GuiManager;

class Core final
{
    bool m_valid;
    bool m_active;

    ConfigManager *m_configManager;
    VRManager *m_vrManager;
    SfmlManager *m_sfmlManager;
    GuiManager *m_guiManager;

    Core(const Core &that) = delete;
    Core& operator=(const Core &that) = delete;
public:
    Core();
    ~Core();

    bool Intitialize();
    void Terminate();

    ConfigManager* GetConfigManager() const;
    VRManager* GetVRManager() const;
    SfmlManager* GetSfmlManager() const;
    GuiManager* GetGuiManager() const;

    bool DoPulse();
};

