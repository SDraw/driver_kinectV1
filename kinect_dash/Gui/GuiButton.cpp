#include "stdafx.h"

#include "Gui/GuiButton.h"

GuiButton::GuiButton(const std::string& p_name) : GuiElement(p_name)
{
    m_clickState = false;
}
GuiButton::~GuiButton()
{
}

void GuiButton::SetClickCallback(const std::function<void(bool)> &p_callback)
{
    m_clickCallback = p_callback;
}

void GuiButton::Update()
{
    GuiElement::Update();

    ImGui::PushStyleColor(ImGuiCol_Button, m_imguiColor);
    bool l_state = ImGui::Button(m_name.c_str(), m_imguiSize);
    ImGui::PopStyleColor(1);
    if(m_clickState != l_state)
    {
        m_clickState = l_state;
        if(m_clickCallback) m_clickCallback(m_clickState);
    }

    GuiElement::UpdateChildren();
}
