#pragma once
#include "Gui/GuiElement.h"

class GuiScreen final : public GuiElement
{
    sf::Clock m_deltaClock;
    sf::Vector2i m_mousePosition;

    GuiScreen(const GuiScreen &that) = delete;
    GuiScreen& operator=(const GuiScreen &that) = delete;
public:
    explicit GuiScreen(const std::string &p_name, const sf::Vector2i &p_size);
    ~GuiScreen();

    void InjectMouseMove(const sf::Vector2f &p_pos);
    void InjectMouseClick(sf::Mouse::Button p_button, bool p_press);

    void Update() override;
    void Render();
};
