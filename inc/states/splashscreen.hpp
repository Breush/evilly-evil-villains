#pragma once

#include "states/state.hpp"
#include "sfe/animatedsprite.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <list>

class SplashScreenState : public State
{
public:
    SplashScreenState(StateStack& stack);
    virtual ~SplashScreenState() {}
    States::ID id() const override { return States::SPLASHSCREEN; }

    void draw() override;
    bool update(const sf::Time& dt) override;
    bool handleEvent(const sf::Event& event) override;

private:
    sf::Sprite m_bgSprite;
    sf::Shader* m_bgShader;
    sfe::AnimatedSprite m_logo;
};
