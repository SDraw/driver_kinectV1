#pragma once
#include "Gui/GuiElement.h"

class GuiRadioButton final : public GuiElement
{
    bool m_active;
    std::function<void(void)> m_clickCallback;

    GuiRadioButton(const GuiRadioButton &that) = delete;
    GuiRadioButton& operator=(const GuiRadioButton &that) = delete;

    // GuiElement
    void Update() override;
public:
    explicit GuiRadioButton(const std::string& f_name);
    ~GuiRadioButton();

    void SetActive(bool f_state);
    bool GetActive() const;

    void SetClickCallback(const std::function<void(void)> &f_callback);
};

