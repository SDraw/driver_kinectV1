#include "stdafx.h"

#include "Core/Core.h"
#include "Managers/SfmlManager.h"
#include "Managers/GuiManager.h"

SFML_DEFINE_DISCRETE_GPU_PREFERENCE;

SfmlManager::SfmlManager(Core *p_core)
{
    m_core = p_core;

    const sf::ContextSettings l_contextSettings(16U, 8U, 0U, 2U, 1U, sf::ContextSettings::Default, false);
    m_context = new sf::Context(l_contextSettings, 1024U, 512U);
    if(!m_context->setActive(true)) throw std::runtime_error("Unable to create SFML context");

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
        // Frame pass
        m_core->GetGuiManager()->Render();
    }
    return m_active;
}
