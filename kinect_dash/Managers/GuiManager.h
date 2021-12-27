#pragma once

class Core;
class GuiScreen;
class GuiWindow;
class GuiButton;
class GuiText;
class GuiInputText;
class GuiSeparator;
class GuiRadioButton;

class GuiManager final
{
    friend Core;
    friend class SfmlManager;
    friend class VRManager;
    Core *m_core;

#ifndef DASHBOARD_DESKTOP
    sf::RenderTexture *m_renderTexture;
#endif

    // Render target is needed
    GuiScreen *m_screen;

    // Trackers toggle
    GuiWindow *m_trackersWindow;
    std::vector<GuiButton*> m_trackersButtons;

    // Calibration info
    GuiWindow *m_calibrationWindow;
    std::vector<GuiInputText*> m_calibrationFields;

    // Settings
    GuiWindow *m_settingsWindow;
    std::vector<GuiButton*> m_settingsButtons;
    GuiSeparator *m_settingsSeparator;
    GuiText *m_interpolationText;
    std::vector<GuiRadioButton*> m_interpolationButtons;
    GuiSeparator *m_deviceSeparator;
    GuiButton *m_deviceStateButton;

    GuiManager(const GuiManager &that) = delete;
    GuiManager& operator=(const GuiManager &that) = delete;

    void OnTrackerToggle(bool p_state, size_t p_index);
    void OnTrackingToggle(bool p_state);
    void OnInterpolationChange(size_t p_type);
    void OnCalibrationStart(bool p_state);
protected:
    explicit GuiManager(Core *p_core);
    ~GuiManager();

#ifndef DASHBOARD_DESKTOP
    unsigned int GetRenderTargetTextureName() const;
#endif

    void Render(); // SfmlManager call only

    // VRManager/SfmlManager only
    void ReceiveMouseMove(float p_x, float p_y);
    void ReceiveMouseClick(bool p_left, bool p_press);
};
