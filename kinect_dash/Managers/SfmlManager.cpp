#include "stdafx.h"

#include "Core/Core.h"
#include "Managers/SfmlManager.h"
#include "Managers/GuiManager.h"

SFML_DEFINE_DISCRETE_GPU_PREFERENCE;

SfmlManager::SfmlManager(Core *f_core)
{
    m_core = f_core;

    //const sf::ContextSettings l_contextSettings(0U, 0U, 0U, 3U, 0U, sf::ContextSettings::Core, false);
    //m_context = new sf::Context(l_contextSettings, 2U, 2U);
    m_context = new sf::Context();
    if(!m_context->setActive(true)) throw std::runtime_error("Unable to bind SFML context");

    m_active = true;
}

SfmlManager::~SfmlManager()
{
    delete m_context;
}

bool SfmlManager::DoPulse()
{
    if(m_active)
    {
        // GUI render
        m_core->GetGuiManager()->Render();
    }
    return m_active;
}
