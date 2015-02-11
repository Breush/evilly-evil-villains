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
    float maxSize = std::max(resolution.x, resolution.y);

    // Background
    const auto& texture = Application::context().textures.get(Textures::JUMPINGTOASTS_BG);
    const auto& textureSize = texture.getSize();
    m_bgSprite.setTexture(texture);
    m_bgSprite.setScale(maxSize / textureSize.x, maxSize / textureSize.y);
    m_bgSprite.setPosition(sf::vsub(resolution, maxSize) / 2.f);

    // Shader
    m_bgShader = &Application::context().shaders.get(Shaders::MENU_BG);

    // Animation
    m_logo.load(Animations::JUMPINGTOASTS);
    m_logo.setPosition(resolution / 2.f);
    m_logo.setLooping(false);
    m_logo.restart();
}

void SplashScreenState::draw()
{
    auto& window = Application::context().window;
    window.setView(Application::context().views.get(Views::DEFAULT));

    // Animated background
    window.draw(m_bgSprite, m_bgShader);

    // Draw animated logo
    window.draw(m_logo);
}

bool SplashScreenState::update(const sf::Time& dt)
{
    // Check on animated entities
    if (!m_logo.started())
        stackPopPush(States::MENU_MAIN);

    return true;
}

bool SplashScreenState::handleEvent(const sf::Event& event)
{
    // Back to previous state on Escape
    if (event.type == sf::Event::KeyPressed
        && event.key.code == sf::Keyboard::Escape) {
        stackPopPush(States::MENU_MAIN);
        Application::context().sounds.stopAll();
    }

    return false;
}
