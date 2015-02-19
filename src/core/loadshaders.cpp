#include "core/application.hpp"

#include "resources/identifiers.hpp"

void Application::loadShaders()
{
    // Default is empty
    s_context.shaders.loadVoid(ShaderID::DEFAULT);

    returnif (!sf::Shader::isAvailable());

    // NUI
    s_context.shaders.load(ShaderID::NUI_HOVER, "res/shd/nui/hover.frag", sf::Shader::Fragment);
    s_context.shaders.load(ShaderID::NUI_FOCUS, "res/shd/nui/focus.frag", sf::Shader::Fragment);

    // Menu
    s_context.shaders.load(ShaderID::MENU_BACKGROUND, "res/shd/menu/bg.vert", "res/shd/menu/bg.frag");
    s_context.shaders.load(ShaderID::MENU_NAME, "res/shd/menu/name.vert", sf::Shader::Vertex);

    refreshShaders();
}

void Application::refreshShaders()
{
    returnif (!sf::Shader::isAvailable());

    const auto& screenSize = s_context.screenSize;
    const auto& resolution = s_context.resolution;
    const auto& effectiveDisplay = s_context.effectiveDisplay;

    // NUI
    s_context.shaders.setParameter(ShaderID::NUI_HOVER, "texture", sf::Shader::CurrentTexture);
    s_context.shaders.setParameter(ShaderID::NUI_FOCUS, "screenSize", screenSize);
    s_context.shaders.setParameter(ShaderID::NUI_FOCUS, "resolution", resolution);
    s_context.shaders.setParameter(ShaderID::NUI_FOCUS, "effectiveDisplay", effectiveDisplay);

    // Menu
    s_context.shaders.setParameter(ShaderID::MENU_BACKGROUND, "screenSize", screenSize);
    s_context.shaders.setParameter(ShaderID::MENU_BACKGROUND, "resolution", resolution);
    s_context.shaders.setParameter(ShaderID::MENU_BACKGROUND, "effectiveDisplay", effectiveDisplay);
    s_context.shaders.setParameter(ShaderID::MENU_BACKGROUND, "texture", sf::Shader::CurrentTexture);
}

void Application::updateShaders(const sf::Time& dt)
{
    s_context.shaders.setParameter(ShaderID::MENU_BACKGROUND, "time", m_gameTime / 5.f);
    s_context.shaders.setParameter(ShaderID::MENU_NAME, "time", m_gameTime / 5.f);
}

