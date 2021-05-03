#include "stdafx.h"

#include "GuiSeparator.h"

GuiSeparator::GuiSeparator() : GuiElement("")
{
}
GuiSeparator::~GuiSeparator()
{
}

void GuiSeparator::Update()
{
    GuiElement::Update();
    ImGui::Separator();
    GuiElement::UpdateChildren();
}
