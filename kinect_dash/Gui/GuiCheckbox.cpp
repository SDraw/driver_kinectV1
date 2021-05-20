#include "stdafx.h"
#include "Gui/GuiCheckbox.h"


GuiCheckbox::GuiCheckbox(const std::string &f_name, bool f_state) : GuiElement(f_name)
{
    m_lastState = f_state;
    m_currentState = f_state;
}
GuiCheckbox::~GuiCheckbox()
{
}

void GuiCheckbox::SetCheckCallback(const std::function<void(bool)> &f_callback)
{
    m_checkCallback = f_callback;
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
