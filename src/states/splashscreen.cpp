#include "states/splashscreen.hpp"
#include "tools/tools.hpp"
#include "resources/holder.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Shader.hpp>

#include <iostream>

SplashScreenState::SplashScreenState(StateStack& stack, Context context)
    : State(stack, context)
    , mText()
    , mShowText(true)
    , mTextEffectTime(sf::Time::Zero)
{
    const sf::Vector2f& windowSize = context.window->getView().getSize();

    // Background
    sf::Texture& texture = context.textures->get(Textures::MENU_BG);
    m_bgSprite.setTexture(texture);
    m_bgSprite.setPosition((windowSize - sf::Vector2f(texture.getSize())) / 2.f);

    // Shader
    m_bgShader = &context.shaders->get(Shaders::MENU_BG);

    // Text
    mText.setFont(context.fonts->get(Fonts::NUI)); // TODO Main font
    mText.setString("Press any key to start");
    mText.setPosition(windowSize / 2.f);
    auto bounds = mText.getLocalBounds();
    mText.setOrigin((bounds.left + bounds.width) / 2.f, (bounds.top + bounds.height) / 2.f);
}

void SplashScreenState::draw()
{
    sf::RenderWindow& window = *getContext().window;
    window.setView(window.getDefaultView());

    // Animated background
    window.draw(m_bgSprite, m_bgShader);

    if (mShowText)
        window.draw(mText);
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
