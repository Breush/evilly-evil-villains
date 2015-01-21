#pragma once

#include "tools/tools.hpp"
#include "tools/param.hpp"

#include "game/game.hpp"
#include "states/state.hpp"

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/System.hpp>

class Player : public sf::Drawable, private sf::NonCopyable
{
public:
    // Constructors
    Player(Game::Context& inGameContext);
    virtual ~Player();

    // Routine
    void init();
    void update(sf::Time dt);
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    // Positions
    void checkOnGround();

    // Params
    PARAMGS(bool, m_onGround, onGround, setOnGround)
    PARAMGS(sf::Vector2f, m_position, position, setPosition)
    PARAMGS(sf::Vector2f, m_speed, speed, setSpeed)
    PARAMGS(sf::Vector2f, m_acceleration, acceleration, setAcceleration)

private:
    Game::Context& m_gameContext;
};
