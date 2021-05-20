#include "stdafx.h"

#include "Gui/GuiLabel.h"

GuiLabel::GuiLabel(const std::string &f_caption) : GuiElement(f_caption)
{
}
GuiLabel::~GuiLabel()
{
}

void GuiLabel::SetText(const std::string &f_text)
{
    m_text.assign(f_text);
}
const std::string& GuiLabel::GetText() const
{
    return m_text;
}

void GuiLabel::Update()
{
    GuiElement::Update();

    ImGui::LabelText(m_name.c_str(), m_text.c_str());

    GuiElement::UpdateChildren();
}
