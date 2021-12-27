#include "stdafx.h"
#include "Gui/GuiCheckbox.h"


GuiCheckbox::GuiCheckbox(const std::string &p_name, bool p_state) : GuiElement(p_name)
{
    m_lastState = p_state;
    m_currentState = p_state;
}
GuiCheckbox::~GuiCheckbox()
{
}

void GuiCheckbox::SetCheckCallback(const std::function<void(bool)> &p_callback)
{
    m_checkCallback = p_callback;
}

void GuiCheckbox::Update()
{
    GuiElement::Update();

    ImGui::Checkbox(m_name.c_str(), &m_currentState);
    if(m_lastState != m_currentState)
    {
        m_lastState = m_currentState;
        if(m_checkCallback) m_checkCallback(m_lastState);
    }
    GuiElement::UpdateChildren();
}
