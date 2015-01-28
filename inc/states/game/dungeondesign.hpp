#pragma once

#include "nui/uicore.hpp"
#include "dungeon/inter.hpp"
#include "states/state.hpp"

class GameDungeonDesignState : public State
{
public:
    GameDungeonDesignState(StateStack& stack);

    virtual void draw();
    virtual bool update(const sf::Time& dt);
    virtual bool handleEvent(const sf::Event& event);

private:
    // NUI
    nui::uiCore m_uiCore;

    // Dungeon
    DungeonInter m_dungeonInter;
};
