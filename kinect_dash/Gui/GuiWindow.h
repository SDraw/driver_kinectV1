#pragma once
#include "Gui/GuiElement.h"

class GuiWindow final : public GuiElement
{
    GuiWindow(const GuiWindow &that) = delete;
    GuiWindow& operator=(const GuiWindow &that) = delete;

    // GuiElement
    void Update() override;
public:
    explicit GuiWindow(const std::string &p_title);
    ~GuiWindow();
};
