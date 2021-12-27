#include "stdafx.h"

#include "Gui/GuiRadioButton.h"


GuiRadioButton::GuiRadioButton(const std::string& p_name) : GuiElement(p_name)
{
    m_active = false;
}
GuiRadioButton::~GuiRadioButton()
{
}

void GuiRadioButton::SetActive(bool p_state)
{
    m_active = p_state;
}
bool GuiRadioButton::GetActive() const
{
    return m_active;
}

void GuiRadioButton::SetClickCallback(const std::function<void(void)> &p_callback)
{
    m_clickCallback = p_callback;
}

void GuiRadioButton::Update()
{
    GuiElement::Update();

    if(ImGui::RadioButton(m_name.c_str(), m_active))
    {
        m_active = true;
        if(m_clickCallback) m_clickCallback();
    }

    GuiElement::UpdateChildren();
}