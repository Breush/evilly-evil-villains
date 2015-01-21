#pragma once

#include "game/game.hpp"
#include "world/world.hpp"
#include "player/player.hpp"
#include "states/state.hpp"

class GameGameState : public State
{
public:
    GameGameState(StateStack& stack);

    virtual void draw();
    virtual bool update(sf::Time dt);
    virtual bool handleEvent(const sf::Event& event);

private:
    Game m_game;
    World m_world;
    Player m_player;
};
