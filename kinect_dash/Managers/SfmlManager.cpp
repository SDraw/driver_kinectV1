#include "stdafx.h"

#include "Core/Core.h"
#include "Managers/SfmlManager.h"
#include "Managers/GuiManager.h"

SFML_DEFINE_DISCRETE_GPU_PREFERENCE;

SfmlManager::SfmlManager(Core *p_core)
{
    m_core = p_core;

    // AMD sucks at OpenGL implementation
    const sf::ContextSettings l_contextSettings(16U, 8U, 0U, 2U, 1U, sf::ContextSettings::Default, false);
#ifdef DASHBOARD_DESKTOP
    const sf::VideoMode l_mode(1024U, 512U);
    m_window = new sf::RenderWindow(l_mode, "KinectV1 Dashboard", sf::Style::Titlebar | sf::Style::Close, l_contextSettings);
    if(!m_window->setActive(true)) throw std::runtime_error("Unable to create SFML window");
    m_window->setFramerateLimit(30U);

    std::memset(&m_event, 0, sizeof(sf::Event));
#else
    m_context = new sf::Context(l_contextSettings, 1024U, 512U);
    if(!m_context->setActive(true)) throw std::runtime_error("Unable to create SFML context");
#endif

    m_active = true;
}

SfmlManager::~SfmlManager()
{
#ifdef DASHBOARD_DESKTOP
    m_window->close();
    delete m_window;
#else
    delete m_context;
#endif
}

#ifdef DASHBOARD_DESKTOP
sf::RenderWindow* SfmlManager::GetWindow() const
{
    return m_window;
}
#endif

bool SfmlManager::DoPulse()
{
    if(m_active)
    {
#ifdef DASHBOARD_DESKTOP
        while(m_window->pollEvent(m_event))
        {
            switch(m_event.type)
            {
                case sf::Event::EventType::Closed:
                    m_active = false;
                    break;
                case sf::Event::EventType::MouseMoved:
                    m_core->GetGuiManager()->ReceiveMouseMove(static_cast<float>(m_event.mouseMove.x), static_cast<float>(m_event.mouseMove.y));
                    break;
                case sf::Event::EventType::MouseButtonPressed: case sf::Event::EventType::MouseButtonReleased:
                    m_core->GetGuiManager()->ReceiveMouseClick(m_event.mouseButton.button == sf::Mouse::Button::Left, m_event.type == sf::Event::EventType::MouseButtonPressed);
                    break;
            }
        }
#endif
        // Frame pass
#ifdef DASHBOARD_DESKTOP
        m_window->clear();
#endif
        m_core->GetGuiManager()->Render();
#ifdef DASHBOARD_DESKTOP
        m_window->display();
#endif
    }
    return m_active;
}
