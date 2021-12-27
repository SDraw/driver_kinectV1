#include "stdafx.h"

#include "Gui/GuiScreen.h"

GuiScreen::GuiScreen(const std::string &p_name, const sf::Vector2i &p_size) : GuiElement(p_name)
{
    m_size.x = static_cast<float>(p_size.x);
    m_size.y = static_cast<float>(p_size.y);
}

GuiScreen::~GuiScreen()
{
}

void GuiScreen::InjectMouseMove(const sf::Vector2f &p_pos)
{
    m_mousePosition.x = static_cast<int>(p_pos.x);
    m_mousePosition.y = static_cast<int>(p_pos.y);

    sf::Event l_event;
    l_event.type = sf::Event::EventType::MouseMoved;
    l_event.mouseMove.x = m_mousePosition.x;
    l_event.mouseMove.y = m_mousePosition.y;
    ImGui::SFML::ProcessEvent(l_event);
}

void GuiScreen::InjectMouseClick(sf::Mouse::Button p_button, bool p_press)
{
    sf::Event l_event;
    l_event.type = (p_press ? sf::Event::EventType::MouseButtonPressed : sf::Event::EventType::MouseButtonReleased);
    l_event.mouseButton.button = p_button;
    l_event.mouseButton.x = m_mousePosition.x;
    l_event.mouseButton.y = m_mousePosition.y;
    ImGui::SFML::ProcessEvent(l_event);
}

void GuiScreen::Update()
{
    ImGui::SFML::Update(m_mousePosition, m_size, m_deltaClock.restart());

    GuiElement::UpdateChildren();
}

void GuiScreen::Render()
{
    ImGui::SFML::Render();
}
