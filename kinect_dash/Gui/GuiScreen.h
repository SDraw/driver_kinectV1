#pragma once
#include "Gui/GuiElement.h"

class GuiScreen final : public GuiElement
{
    sf::Clock m_deltaClock;
    sf::Vector2i m_mousePosition;

    GuiScreen(const GuiScreen &that) = delete;
    GuiScreen& operator=(const GuiScreen &that) = delete;
public:
    explicit GuiScreen(const std::string &f_name, const sf::Vector2i &f_size);
    ~GuiScreen();

    void InjectMouseMove(const sf::Vector2f &f_pos);
    void InjectMouseClick(sf::Mouse::Button f_button, bool f_press);

    void Update() override;
    void Render();
};
