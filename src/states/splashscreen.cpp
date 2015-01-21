#include "states/splashscreen.hpp"

#include "core/application.hpp"
#include "tools/tools.hpp"
#include "resources/holder.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Shader.hpp>

#include <iostream>

SplashScreenState::SplashScreenState(StateStack& stack)
    : State(stack)
    , mText()
    , mShowText(true)
    , mTextEffectTime(sf::Time::Zero)
{
    const sf::Vector2f& windowSize = Application::context().resolution;

    // Background
    sf::Texture& texture = Application::context().textures.get(Textures::MENU_BG);
    m_bgSprite.setTexture(texture);
    m_bgSprite.setPosition((windowSize - sf::Vector2f(texture.getSize())) / 2.f);

    // Shader
    m_bgShader = &Application::context().shaders.get(Shaders::MENU_BG);

    // Text
    mText.setFont(Application::context().fonts.get(Fonts::NUI)); // TODO Main font
    mText.setString("Press any key to start");
    mText.setPosition(windowSize / 2.f);
    auto bounds = mText.getLocalBounds();
    mText.setOrigin((bounds.left + bounds.width) / 2.f, (bounds.top + bounds.height) / 2.f);
}

void SplashScreenState::draw()
{
    // Animated background
    Application::context().window.draw(m_bgSprite, m_bgShader);

    if (mShowText)
        Application::context().window.draw(mText);
}

bool SplashScreenState::update(sf::Time dt)
{
    mTextEffectTime += dt;

    if (mTextEffectTime >= sf::seconds(0.5f)) {
        mShowText = !mShowText;
        mTextEffectTime = sf::Time::Zero;
    }

    return true;
}

bool SplashScreenState::handleEvent(const sf::Event& event)
{
    // If any key is pressed, trigger the next screen
    if(event.type == sf::Event::KeyPressed) {
        stackPopPush(States::MENU_MAIN);
    }

    return true;
}
