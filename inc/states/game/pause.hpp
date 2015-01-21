#pragma once

#include "states/state.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>


class GamePauseState : public State
{
public:
    GamePauseState(StateStack& stack);

    virtual void draw();
    virtual bool update(sf::Time dt);
    virtual bool handleEvent(const sf::Event& event);


private:
    sf::Sprite mBackgroundSprite;
    sf::Text   mPausedText;
    sf::Text   mInstructionText;
};
