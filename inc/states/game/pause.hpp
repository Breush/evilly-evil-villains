#pragma once

#include "states/state.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>


class GamePauseState : public State
{
public:
    GamePauseState(StateStack& stack);

    void draw() override;
    bool update(const sf::Time& dt) override;
    bool handleEvent(const sf::Event& event) override;

private:
    sf::RectangleShape m_bg;
    sf::Text   m_pausedText;
    sf::Text   m_instructionText;
};
