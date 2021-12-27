#pragma once
#include "Gui/GuiElement.h"

class GuiCheckbox final : public GuiElement
{
    bool m_lastState;
    bool m_currentState;
    std::function<void(bool)> m_checkCallback;

    GuiCheckbox(const GuiCheckbox &that) = delete;
    GuiCheckbox& operator=(const GuiCheckbox &that) = delete;

    // GuiElement
    void Update() override;
public:
    explicit GuiCheckbox(const std::string &p_name, bool p_state = false);
    ~GuiCheckbox();

    void SetCheckCallback(const std::function<void(bool)> &p_callback);
};

