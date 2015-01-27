#include "states/splashscreen.hpp"

#include "core/application.hpp"
#include "tools/tools.hpp"
#include "resources/holder.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Shader.hpp>

#include <iostream>

SplashScreenState::SplashScreenState(StateStack& stack)
    : State(stack)
{
    const sf::Vector2f& windowSize = Application::context().resolution;
    float maxSize = std::max(windowSize.x, windowSize.y);

    // Background
    const auto& texture = Application::context().textures.get(Textures::JUMPINGTOASTS_BG);
    const auto& textureSize = texture.getSize();
    m_bgSprite.setTexture(texture);
    m_bgSprite.setTextureRect({0, 0, int(maxSize), int(maxSize)});
    m_bgSprite.setPosition(sf::vsub(windowSize, maxSize) / 2.f);

    // Shader
    m_bgShader = &Application::context().shaders.get(Shaders::MENU_BG);

    // Animation
    m_logo.load(Animations::JUMPINGTOASTS);
    m_logo.setPosition(windowSize / 2.f);
    m_logo.setLooping(false);
    m_logo.restart();
}

SplashScreenState::~SplashScreenState()
{
}

void SplashScreenState::draw()
{
    auto& window = Application::context().window;
    const auto& resolution = Application::context().resolution;

    // Animated background
    window.draw(m_bgSprite, m_bgShader);

    // Draw animated logo
    window.draw(m_logo);
}

bool SplashScreenState::update(sf::Time dt)
{
    // Check on animated entities
    if (!m_logo.started())
        stackPopPush(States::MENU_MAIN);

    return true;
}

bool SplashScreenState::handleEvent(const sf::Event& event)
{
    return true;
}
