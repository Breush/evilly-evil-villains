#include "states/splashscreen.hpp"

#include "core/application.hpp"
#include "resources/identifiers.hpp"
#include "tools/tools.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Shader.hpp>

using namespace states;

SplashScreen::SplashScreen(StateStack& stack)
    : State(stack)
{
    const sf::Vector2f& resolution = Application::context().resolution;
    float maxSide = std::max(resolution.x, resolution.y);

    // Background
    const auto& textureSize = Application::context().textures.get(TextureID::JUMPINGTOASTS_BACKGROUND).getSize();
    sceneLayer(LayerID::NUI).attachChild(m_background);
    m_background.setDepth(100.f);
    m_background.setTexture(TextureID::JUMPINGTOASTS_BACKGROUND);
    m_background.setShader(ShaderID::MENU_BACKGROUND);
    m_background.setLocalScale({maxSide / textureSize.x, maxSide / textureSize.y});
    m_background.setLocalPosition(sf::vsub(resolution, maxSide) / 2.f);

    // Animation
    sceneLayer(LayerID::NUI).attachChild(m_logo);
    m_logo.load(AnimationID::JUMPINGTOASTS);
    m_logo.setLocalPosition(resolution / 2.f);
    m_logo.setLooping(false);
    m_logo.restart();
}

bool SplashScreen::update(const sf::Time& dt)
{
    // Check on animated entities
    if (!m_logo.started())
        stackPopPush(StateID::MENU_MAIN);

    return State::update(dt);
}

bool SplashScreen::handleEvent(const sf::Event& event)
{
    // Back to previous state on Escape
    if (event.type == sf::Event::KeyPressed
        && event.key.code == sf::Keyboard::Escape) {
        stackPopPush(StateID::MENU_MAIN);
        Application::context().sounds.stopAll();
    }

    return State::handleEvent(event);
}
