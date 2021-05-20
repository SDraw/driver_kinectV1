#include "stdafx.h"
#include "Gui/GuiText.h"


GuiText::GuiText(const std::string &f_name) : GuiElement(f_name)
{
}
GuiText::~GuiText()
{
}

void GuiText::Update()
{
    GuiElement::Update();

    ImGui::Text(m_name.c_str());

    GuiElement::UpdateChildren();
}
