#include "stdafx.h"

#include "Gui/GuiRadioButton.h"


GuiRadioButton::GuiRadioButton(const std::string& f_name) : GuiElement(f_name)
{
    m_active = false;
}
GuiRadioButton::~GuiRadioButton()
{
}

void GuiRadioButton::SetActive(bool f_state)
{
    m_active = f_state;
}
bool GuiRadioButton::GetActive() const
{
    return m_active;
}

void GuiRadioButton::SetClickCallback(const std::function<void(void)> &f_callback)
{
    m_clickCallback = f_callback;
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