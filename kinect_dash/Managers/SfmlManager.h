#pragma once

class Core;

class SfmlManager final
{
    friend Core;
    Core *m_core;

    sf::Context *m_context;
    bool m_active;

    SfmlManager(const SfmlManager &that) = delete;
    SfmlManager& operator=(const SfmlManager &that) = delete;
public:
protected:
    explicit SfmlManager(Core *f_core);
    ~SfmlManager();

    bool DoPulse();
};
