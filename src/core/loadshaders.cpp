#include "core/application.hpp"

void Application::loadShaders()
{
    // Default is empty
    s_context.shaders.loadVoid(Shaders::DEFAULT);

    returnif (!sf::Shader::isAvailable());

    // NUI
    s_context.shaders.load(Shaders::NUI_HOVER, "res/shd/nui/hover.frag", sf::Shader::Fragment);
    s_context.shaders.load(Shaders::NUI_FOCUS, "res/shd/nui/focus.frag", sf::Shader::Fragment);

    // Menu
    s_context.shaders.load(Shaders::MENU_BG, "res/shd/menu/bg.vert", "res/shd/menu/bg.frag");
    s_context.shaders.load(Shaders::MENU_NAME, "res/shd/menu/name.vert", sf::Shader::Vertex);

    refreshShaders();
}

void Application::refreshShaders()
{
    returnif (!sf::Shader::isAvailable());

    const auto& screenSize = s_context.screenSize;
    const auto& resolution = s_context.resolution;
    const auto& effectiveDisplay = s_context.effectiveDisplay;

    // NUI
    s_context.shaders.setParameter(Shaders::NUI_HOVER, "texture", sf::Shader::CurrentTexture);
    s_context.shaders.setParameter(Shaders::NUI_FOCUS, "screenSize", screenSize);
    s_context.shaders.setParameter(Shaders::NUI_FOCUS, "resolution", resolution);
    s_context.shaders.setParameter(Shaders::NUI_FOCUS, "effectiveDisplay", effectiveDisplay);

    // Menu
    s_context.shaders.setParameter(Shaders::MENU_BG, "screenSize", screenSize);
    s_context.shaders.setParameter(Shaders::MENU_BG, "resolution", resolution);
    s_context.shaders.setParameter(Shaders::MENU_BG, "effectiveDisplay", effectiveDisplay);
    s_context.shaders.setParameter(Shaders::MENU_BG, "texture", sf::Shader::CurrentTexture);
}

void Application::updateShaders(const sf::Time& dt)
{
    s_context.shaders.setParameter(Shaders::MENU_BG, "time", m_gameTime / 5.f);
    s_context.shaders.setParameter(Shaders::MENU_NAME, "time", m_gameTime / 5.f);
}

