#pragma once

class Core;

class SfmlManager final
{
    friend Core;
    Core *m_core;

#ifdef DASHBOARD_DESKTOP
    sf::RenderWindow *m_window;
    sf::Event m_event;
#else
    sf::Context *m_context;
#endif
    bool m_active;

    SfmlManager(const SfmlManager &that) = delete;
    SfmlManager& operator=(const SfmlManager &that) = delete;
public:
#ifdef DASHBOARD_DESKTOP
    sf::RenderWindow* GetWindow() const;
#endif
protected:
    explicit SfmlManager(Core *p_core);
    ~SfmlManager();

    bool DoPulse();
};
