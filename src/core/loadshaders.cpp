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
    s_context.shaders.load(ShaderID::JUMPINGTOASTS_BACKGROUND, "res/shd/menu/splashscreen.vert", "res/shd/menu/splashscreen.frag");
    s_context.shaders.load(ShaderID::MENU_MAIN_BACKGROUND, "res/shd/menu/background.vert", "res/shd/menu/background.frag");
    s_context.shaders.load(ShaderID::MENU_MAIN_LOGO, "res/shd/menu/name.vert", sf::Shader::Vertex);

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

    // Splashcreen
    s_context.shaders.setParameter(ShaderID::JUMPINGTOASTS_BACKGROUND, "screenSize", screenSize);
    s_context.shaders.setParameter(ShaderID::JUMPINGTOASTS_BACKGROUND, "resolution", resolution);
    s_context.shaders.setParameter(ShaderID::JUMPINGTOASTS_BACKGROUND, "effectiveDisplay", effectiveDisplay);
    s_context.shaders.setParameter(ShaderID::JUMPINGTOASTS_BACKGROUND, "texture", sf::Shader::CurrentTexture);

    // Menu
    s_context.shaders.setParameter(ShaderID::MENU_MAIN_BACKGROUND, "resolution", resolution);
}

void Application::updateShaders(const sf::Time& dt)
{
    s_context.shaders.setParameter(ShaderID::JUMPINGTOASTS_BACKGROUND, "time", m_gameTime / 5.f);
    s_context.shaders.setParameter(ShaderID::MENU_MAIN_BACKGROUND, "time", m_gameTime / 25.f);
    s_context.shaders.setParameter(ShaderID::MENU_MAIN_LOGO, "time", m_gameTime / 5.f);
}

