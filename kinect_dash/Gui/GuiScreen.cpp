#include "stdafx.h"

#include "Gui/GuiScreen.h"

GuiScreen::GuiScreen(const std::string &f_name, const sf::Vector2i &f_size) : GuiElement(f_name)
{
    m_size.x = static_cast<float>(f_size.x);
    m_size.y = static_cast<float>(f_size.y);
}

GuiScreen::~GuiScreen()
{
}

void GuiScreen::InjectMouseMove(const sf::Vector2f &f_pos)
{
    m_mousePosition.x = static_cast<int>(f_pos.x);
    m_mousePosition.y = static_cast<int>(f_pos.y);

    sf::Event l_event;
    l_event.type = sf::Event::EventType::MouseMoved;
    l_event.mouseMove.x = m_mousePosition.x;
    l_event.mouseMove.y = m_mousePosition.y;
    ImGui::SFML::ProcessEvent(l_event);
}

void GuiScreen::InjectMouseClick(sf::Mouse::Button f_button, bool f_press)
{
    sf::Event l_event;
    l_event.type = (f_press ? sf::Event::EventType::MouseButtonPressed : sf::Event::EventType::MouseButtonReleased);
    l_event.mouseButton.button = f_button;
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
