#pragma once
#include "Gui/GuiElement.h"

class GuiSeparator final : public GuiElement
{
    GuiSeparator(const GuiSeparator &that) = delete;
    GuiSeparator& operator=(const GuiSeparator &that) = delete;

    // GuiElement
    void Update() override;
public:
    GuiSeparator();
    ~GuiSeparator();
};

