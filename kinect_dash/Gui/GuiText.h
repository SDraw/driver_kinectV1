#pragma once
#include "Gui/GuiElement.h"

class GuiText final : public GuiElement
{
    GuiText(const GuiText &that) = delete;
    GuiText& operator=(const GuiText &that) = delete;

    // GuiElement
    void Update() override;
public:
    explicit GuiText(const std::string &p_name);
    ~GuiText();
};
