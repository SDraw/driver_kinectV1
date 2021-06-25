#include "stdafx.h"

#include "Core/Core.h"
#include "Managers/ConfigManager.h"
#include "Managers/GuiManager.h"
#include "Managers/SfmlManager.h"
#include "Managers/VRManager.h"

#include "Gui/GuiScreen.h"
#include "Gui/GuiWindow.h"
#include "Gui/GuiButton.h"
#include "Gui/GuiText.h"
#include "Gui/GuiInputText.h"
#include "Gui/GuiSeparator.h"
#include "Gui/GuiRadioButton.h"

extern const size_t g_bonesCount;

const std::string g_boneNames[]
{
    "Hip (Center)", "Spine", "Neck", "Head",
        "Left Shoulder", "Left Elbow", "Left Wrist", "Left Hand",
        "Right Shoulder", "Right Elbow", "Right Wrist", "Right Hand",
        "Left Hip", "Left Knee", "Left Ankle", "Left Foot",
        "Right Hip", "Right Knee", "Right Ankle", "Right Foot"
};

const std::string g_calibrationNames[]
{
    "Base rotation X", "Base position X",
        "Base rotation Y", "Base position Y",
        "Base rotation Z", "Base position Z",
        "Base rotation W"
};

const std::string g_interpolationNames[]
{
    "Linear", "Quadratic", "Cubic", "Quartic", "Quintic", "Exponential", "Sine", "Circular"
};

enum SettingsButton : size_t
{
    SB_StartCalibration = 0U,
    SB_ToggleTracking,

    SB_Count
};

const sf::Color g_untoggledColor(32, 32, 32, 255);
const sf::Color g_activeColor(0, 127, 0, 255);
const sf::Color g_inactiveColor(127, 0, 0, 255);

GuiManager::GuiManager(Core *f_core)
{
    m_core = f_core;

#ifndef DASHBOARD_DESKTOP
    m_renderTexture = new sf::RenderTexture();
    if(!m_renderTexture->create(1024, 512)) throw std::runtime_error("Unable to create render target for GUI overlay");

    sf::Window l_dummyWindow; // imgui-sfml checks only window focus
    ImGui::SFML::Init(l_dummyWindow, *m_renderTexture, false);
#else
    ImGui::SFML::Init(*m_core->GetSfmlManager()->GetWindow(), sf::Vector2f(1024.f, 512.f), false);
#endif

    sf::Event l_event;
    l_event.type = sf::Event::EventType::GainedFocus;
    ImGui::SFML::ProcessEvent(l_event);

    // Set theme
    ImGui::GetStyle().WindowRounding = 5.0f;
    ImGui::GetStyle().ScrollbarRounding = 5.0f;
    ImGui::GetStyle().GrabRounding = 5.0f;
    ImGui::GetStyle().AntiAliasedLines = true;
    ImGui::GetStyle().AntiAliasedFill = true;
    ImGui::GetStyle().WindowRounding = 5;
    ImGui::GetStyle().ChildRounding = 5;
    ImGui::GetStyle().ScrollbarSize = 16;
    ImGui::GetStyle().ScrollbarRounding = 5;
    ImGui::GetStyle().GrabRounding = 5;
    ImGui::GetStyle().ItemSpacing.x = 10;
    ImGui::GetStyle().ItemSpacing.y = 4;
    ImGui::GetStyle().IndentSpacing = 22;
    ImGui::GetStyle().FramePadding.x = 6;
    ImGui::GetStyle().FramePadding.y = 4;
    ImGui::GetStyle().Alpha = 1.0f;
    ImGui::GetStyle().FrameRounding = 5.0f;

    ImGui::GetStyle().Colors[ImGuiCol_TitleBg] = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
    ImGui::GetStyle().Colors[ImGuiCol_TitleBgActive] = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];

    // Change font
    ImGui::GetIO().Fonts->AddFontFromFileTTF("../../resources/fonts/Hack-Regular.ttf", 14.0f);
    ImGui::SFML::UpdateFontTexture();

    m_screen = new GuiScreen("KinectV1 Dashboard", sf::Vector2i(1024, 512));

    //// Trackers toggle
    m_trackersWindow = new GuiWindow("Trackers Toggle");
    m_trackersWindow->SetPosition(sf::Vector2f(16, 48));
    m_trackersWindow->SetSize(sf::Vector2f(658, 214));
    m_screen->Add(m_trackersWindow);

    for(size_t i = 0U; i < g_bonesCount; i++)
    {
        m_trackersButtons.push_back(new GuiButton(g_boneNames[i].c_str()));

        m_trackersButtons[i]->SetSize(sf::Vector2f(153.f, 32.f));
        m_trackersButtons[i]->SetOnSameLine(i % 4U != 0U);

        bool l_state = m_core->GetConfigManager()->GetTrackerState(i);
        m_trackersButtons[i]->SetColor(l_state ? g_activeColor : g_untoggledColor);
        m_trackersButtons[i]->SetClickCallback(std::bind(&GuiManager::OnTrackerToggle, this, std::placeholders::_1, i));

        m_trackersWindow->Add(m_trackersButtons[i]);
    }

    // Calibration info
    m_calibrationWindow = new GuiWindow("Calibration Info");
    m_calibrationWindow->SetPosition(sf::Vector2f(89.f, 278.f));
    m_calibrationWindow->SetSize(sf::Vector2f(512.f, 136.f));
    m_screen->Add(m_calibrationWindow);

    for(size_t i = 0; i < 7; i++)
    {
        GuiInputText *l_inputBox = new GuiInputText(g_calibrationNames[i]);
        l_inputBox->SetSize(sf::Vector2f(124.f, 28.f));
        l_inputBox->SetOnSameLine(i % 2U != 0U);
        l_inputBox->SetSidesSwitched(true);
        m_calibrationFields.push_back(l_inputBox);
        m_calibrationWindow->Add(l_inputBox);
    }
    for(size_t i = 0U; i < 4; i++) m_calibrationFields[i * 2]->SetText(std::to_string(m_core->GetConfigManager()->GetBaseRotation()[i]));
    for(size_t i = 0U; i < 3; i++) m_calibrationFields[i * 2 + 1]->SetText(std::to_string(m_core->GetConfigManager()->GetBasePosition()[i]));

    // Settings
    m_settingsWindow = new GuiWindow("Settings");
    m_settingsWindow->SetPosition(sf::Vector2f(690.f, 48.f));
    m_settingsWindow->SetSize(sf::Vector2f(310.f, 388.f));
    m_screen->Add(m_settingsWindow);

    m_settingsButtons.push_back(new GuiButton("Start calibration"));
    m_settingsButtons[SettingsButton::SB_StartCalibration]->SetPosition(sf::Vector2f(16.f, 48.f));
    m_settingsButtons[SettingsButton::SB_StartCalibration]->SetSize(sf::Vector2f(294.f, 32.f));
    m_settingsButtons[SettingsButton::SB_StartCalibration]->SetColor(g_untoggledColor);
    m_settingsButtons[SettingsButton::SB_StartCalibration]->SetClickCallback(std::bind(&GuiManager::OnCalibrationStart, this, std::placeholders::_1));
    m_settingsWindow->Add(m_settingsButtons[SettingsButton::SB_StartCalibration]);

    m_settingsButtons.push_back(new GuiButton("Toggle tracking"));
    m_settingsButtons[SettingsButton::SB_ToggleTracking]->SetPosition(sf::Vector2f(16.f, 88.f));
    m_settingsButtons[SettingsButton::SB_ToggleTracking]->SetSize(sf::Vector2f(294.f, 32.f));
    m_settingsButtons[SettingsButton::SB_ToggleTracking]->SetColor(m_core->GetConfigManager()->GetTrackingState() ? g_activeColor : g_inactiveColor);
    m_settingsButtons[SettingsButton::SB_ToggleTracking]->SetClickCallback(std::bind(&GuiManager::OnTrackingToggle, this, std::placeholders::_1));
    m_settingsWindow->Add(m_settingsButtons[SettingsButton::SB_ToggleTracking]);

    m_settingsSeparator = new GuiSeparator();
    m_settingsWindow->Add(m_settingsSeparator);

    m_interpolationText = new GuiText("\nInterpolation type");
    m_settingsWindow->Add(m_interpolationText);

    for(size_t i = 0U; i < 8U; i++)
    {
        m_interpolationButtons.push_back(new GuiRadioButton(g_interpolationNames[i]));
        m_interpolationButtons[i]->SetClickCallback(std::bind(&GuiManager::OnInterpolationChange, this, i));
        m_interpolationButtons[i]->SetActive(m_core->GetConfigManager()->GetInterpolationType() == i);
        m_settingsWindow->Add(m_interpolationButtons[i]);
    }

    m_deviceSeparator = new GuiSeparator();
    m_settingsWindow->Add(m_deviceSeparator);

    std::string l_status("Kinect relay device: ");
    l_status.append(m_core->GetVRManager()->IsKinectDevicePresent() ? "Present" : "Not found");
    m_deviceStateButton = new GuiButton(l_status);
    m_deviceStateButton->SetSize(sf::Vector2f(294.f, 32.f));
    m_deviceStateButton->SetColor(m_core->GetVRManager()->IsKinectDevicePresent() ? g_activeColor : g_inactiveColor);
    m_settingsWindow->Add(m_deviceStateButton);
}
GuiManager::~GuiManager()
{
    // Settings
    delete m_deviceStateButton;
    delete m_deviceSeparator;
    for(size_t i = 0U; i < 8U; i++) delete m_interpolationButtons[i];
    m_interpolationButtons.clear();
    delete m_interpolationText;
    delete m_settingsSeparator;
    delete m_settingsWindow;
    for(size_t i = 0U; i < SettingsButton::SB_Count; i++) delete m_settingsButtons[i];
    m_settingsButtons.clear();

    // Calibration
    for(size_t i = 0; i < 7; i++) delete m_calibrationFields[i];
    m_calibrationFields.clear();
    delete m_calibrationWindow;

    // Trackers toggle
    for(size_t i = 0U; i < g_bonesCount; i++) delete m_trackersButtons[i];
    m_trackersButtons.clear();
    delete m_trackersWindow;

    // Screen
    delete m_screen;
    ImGui::SFML::Shutdown();

#ifndef DASHBOARD_DESKTOP
    delete m_renderTexture;
#endif
}

void GuiManager::Render()
{
#ifndef DASHBOARD_DESKTOP
    if(m_core->GetVRManager()->IsOverlayVisible())
    {
#endif
        // Update calibration info
        for(size_t i = 0U; i < 4; i++) m_calibrationFields[i * 2]->SetText(std::to_string(m_core->GetConfigManager()->GetBaseRotation()[i]));
        for(size_t i = 0U; i < 3; i++) m_calibrationFields[i * 2 + 1]->SetText(std::to_string(m_core->GetConfigManager()->GetBasePosition()[i]));

        // Update gui elements
        m_screen->Update();

#ifndef DASHBOARD_DESKTOP
        m_renderTexture->setActive(true);
        m_renderTexture->clear();
#endif
        m_screen->Render();
#ifndef DASHBOARD_DESKTOP
        m_renderTexture->display();
        m_renderTexture->setActive(false);
    }
#endif
}

#ifndef DASHBOARD_DESKTOP
unsigned int GuiManager::GetRenderTargetTextureName() const
{
    unsigned int l_result = 0U;
    if(m_renderTexture) l_result = m_renderTexture->getTexture().getNativeHandle();
    return l_result;
}
#endif

void GuiManager::ReceiveMouseMove(float f_x, float f_y)
{
    if(m_screen) m_screen->InjectMouseMove(sf::Vector2f(f_x, f_y));
}
void GuiManager::ReceiveMouseClick(bool f_left, bool f_press)
{
    if(m_screen) m_screen->InjectMouseClick(f_left ? sf::Mouse::Button::Left : sf::Mouse::Button::Right, f_press);
}

void GuiManager::OnTrackerToggle(bool f_state, size_t f_index)
{
    if(f_state)
    {
        bool l_newTrackerState = !m_core->GetConfigManager()->GetTrackerState(f_index);
        m_trackersButtons[f_index]->SetColor(l_newTrackerState ? g_activeColor : g_untoggledColor);
        m_core->GetVRManager()->SendTrackerToggle(f_index);
        m_core->GetConfigManager()->ChangeTrackerState(f_index);
    }
}

void GuiManager::OnTrackingToggle(bool f_state)
{
    if(f_state)
    {
        bool l_newTrackingState = !m_core->GetConfigManager()->GetTrackingState();
        m_settingsButtons[SettingsButton::SB_ToggleTracking]->SetColor(l_newTrackingState ? g_activeColor : g_inactiveColor);
        m_core->GetVRManager()->SendTrackingToggle();
        m_core->GetConfigManager()->ChangeTrackingState();
    }
}

void GuiManager::OnInterpolationChange(size_t f_type)
{
    for(size_t i = 0U; i < 8U; i++)
    {
        if(i != f_type) m_interpolationButtons[i]->SetActive(false);
    }

    m_core->GetVRManager()->SendInterpolationChange(f_type);
    m_core->GetConfigManager()->SetInterpolationType(f_type);
}

void GuiManager::OnCalibrationStart(bool f_state)
{
    if(f_state) m_core->GetVRManager()->LaunchCalibration();
}
