#include "stdafx.h"

#include "Gui/GuiInputText.h"

GuiInputText::GuiInputText(const std::string &p_name) : GuiElement(p_name)
{
    m_sidesSwitched = false;
}
GuiInputText::~GuiInputText()
{
}

void GuiInputText::SetText(const std::string &p_text)
{
    m_text.assign(p_text);
}
const std::string& GuiInputText::GetText() const
{
    return m_text;
}

void GuiInputText::SetSidesSwitched(bool p_state)
{
    m_sidesSwitched = p_state;
}
bool GuiInputText::GetSidesSwitched() const
{
    return m_sidesSwitched;
}

void GuiInputText::Update()
{
    GuiElement::Update();

    if(m_sidesSwitched)
    {
        ImGui::Text(m_name.c_str());
        ImGui::SameLine();
    }

    ImGui::PushItemWidth(m_size.x);
    ImGui::InputText(m_sidesSwitched ? "" : m_name.c_str(), &m_text[0U], m_text.size(), ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly);
    ImGui::PopItemWidth();

    GuiElement::UpdateChildren();
}
