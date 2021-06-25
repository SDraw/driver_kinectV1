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

    void OnTrackerToggle(bool f_state, size_t f_index);
    void OnTrackingToggle(bool f_state);
    void OnInterpolationChange(size_t f_type);
    void OnCalibrationStart(bool f_state);
protected:
    explicit GuiManager(Core *f_core);
    ~GuiManager();

#ifndef DASHBOARD_DESKTOP
    unsigned int GetRenderTargetTextureName() const;
#endif

    void Render(); // SfmlManager call only

    // VRManager/SfmlManager only
    void ReceiveMouseMove(float f_x, float f_y);
    void ReceiveMouseClick(bool f_left, bool f_press);
};
