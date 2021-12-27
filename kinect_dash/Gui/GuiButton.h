#pragma once
#include "Gui/GuiElement.h"

class GuiButton final : public GuiElement
{
    std::function<void(bool)> m_clickCallback;
    bool m_clickState;

    GuiButton(const GuiButton &that) = delete;
    GuiButton& operator=(const GuiButton &that) = delete;

    // GuiElement
    void Update() override;
public:
    explicit GuiButton(const std::string &p_name);
    ~GuiButton();

    void SetClickCallback(const std::function<void(bool)> &p_callback);
};
