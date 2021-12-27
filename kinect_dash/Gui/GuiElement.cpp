#include "stdafx.h"

#include "Gui/GuiElement.h"


GuiElement::GuiElement(const std::string &p_name)
{
    m_name.assign(p_name);
    m_position = sf::Vector2f(0.f,0.f);
    m_imguiPosition = ImVec2(0.f, 0.f);
    m_size = sf::Vector2f(0.f,0.f);
    m_imguiSize = ImVec2(0.f, 0.f);
    m_color = sf::Color(0, 0, 0, 255);
    m_imguiColor = ImVec4(0, 0, 0, 255);

    m_sameLine = false;

    m_parent = nullptr;
}
GuiElement::~GuiElement()
{
}

bool GuiElement::Add(GuiElement *p_child)
{
    if(p_child->m_parent == nullptr)
    {
        m_children.push_back(p_child);
        p_child->m_parent = this;
    }

    return (p_child->m_parent == this);
}

void GuiElement::Remove(GuiElement *p_child)
{
    if(p_child->m_parent == this)
    {
        p_child->m_parent = nullptr;
        m_children.erase(std::remove(m_children.begin(), m_children.end(), p_child), m_children.end());
    }
}

bool GuiElement::HasChild(const GuiElement *p_child)
{
    return (std::find(m_children.begin(), m_children.end(), p_child) != m_children.end());
}

void GuiElement::SetPosition(const sf::Vector2f &p_pos)
{
    m_position = p_pos;
    m_imguiPosition.x = m_position.x;
    m_imguiPosition.y = m_position.y;
}
const sf::Vector2f& GuiElement::GetPosition() const
{
    return m_position;
}

void GuiElement::SetSize(const sf::Vector2f &p_size)
{
    m_size = p_size;
    m_imguiSize.x = m_size.x;
    m_imguiSize.y = m_size.y;
}
const sf::Vector2f& GuiElement::GetSize() const
{
    return m_size;
}

void GuiElement::SetColor(const sf::Color &p_col)
{
    m_color = p_col;
    m_imguiColor.x = static_cast<float>(m_color.r) / 255.f;
    m_imguiColor.y = static_cast<float>(m_color.g) / 255.f;
    m_imguiColor.z = static_cast<float>(m_color.b) / 255.f;
    m_imguiColor.w = static_cast<float>(m_color.a) / 255.f;
}
const sf::Color& GuiElement::GetColor() const
{
    return m_color;
}

void GuiElement::SetOnSameLine(bool p_state)
{
    m_sameLine = p_state;
}
bool GuiElement::GetOnSameLine() const
{
    return m_sameLine;
}

void GuiElement::Update()
{
    if(m_sameLine) ImGui::SameLine();
}

void GuiElement::UpdateChildren()
{
    for(auto l_child : m_children) l_child->Update();
}

