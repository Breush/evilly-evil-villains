#pragma once

#include "states/state.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>


class SplashScreenState : public State
{
public:
    SplashScreenState(StateStack& stack);

    virtual void draw();
    virtual bool update(sf::Time dt);
    virtual bool handleEvent(const sf::Event& event);


private:
    sf::Sprite m_bgSprite;
    sf::Shader* m_bgShader;

    // TODO Conventionize
    sf::Text   mText;

    bool mShowText;
    sf::Time mTextEffectTime;
};
