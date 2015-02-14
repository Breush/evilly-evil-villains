#include "states/splashscreen.hpp"

#include "core/application.hpp"
#include "tools/tools.hpp"
#include "resources/holder.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Shader.hpp>

SplashScreenState::SplashScreenState(StateStack& stack)
    : State(stack)
{
    const sf::Vector2f& resolution = Application::context().resolution;
    float maxSide = std::max(resolution.x, resolution.y);

    // Background
    const auto& textureSize = Application::context().textures.get(Textures::JUMPINGTOASTS_BACKGROUND).getSize();
    sceneLayer(Layers::NUI).attachChild(m_background);
    m_background.setDepth(100.f);
    m_background.setTexture(Textures::JUMPINGTOASTS_BACKGROUND);
    m_background.setShader(Shaders::MENU_BACKGROUND);
    m_background.setLocalScale({maxSide / textureSize.x, maxSide / textureSize.y});
    m_background.setLocalPosition(sf::vsub(resolution, maxSide) / 2.f);

    // Animation
    sceneLayer(Layers::NUI).attachChild(m_logo);
    m_logo.load(Animations::JUMPINGTOASTS);
    m_logo.setLocalPosition(resolution / 2.f);
    m_logo.setLooping(false);
    m_logo.restart();
}

bool SplashScreenState::update(const sf::Time& dt)
{
    // Check on animated entities
    if (!m_logo.started())
        stackPopPush(States::MENU_MAIN);

    return State::update(dt);
}

bool SplashScreenState::handleEvent(const sf::Event& event)
{
    // Back to previous state on Escape
    if (event.type == sf::Event::KeyPressed
        && event.key.code == sf::Keyboard::Escape) {
        stackPopPush(States::MENU_MAIN);
        Application::context().sounds.stopAll();
    }

    return State::handleEvent(event);
}
