#include "stdafx.h"

#include "Gui/GuiLabel.h"

GuiLabel::GuiLabel(const std::string &p_caption) : GuiElement(p_caption)
{
}
GuiLabel::~GuiLabel()
{
}

void GuiLabel::SetText(const std::string &p_text)
{
    m_text.assign(p_text);
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
