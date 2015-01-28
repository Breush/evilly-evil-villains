#pragma once

#include "nui/uicore.hpp"
#include "dungeon/data.hpp"
#include "dungeon/inter.hpp"
#include "states/state.hpp"

class GameDungeonDesignState : public State
{
public:
    GameDungeonDesignState(StateStack& stack);
    virtual ~GameDungeonDesignState() {}

    virtual void draw();
    virtual bool update(const sf::Time& dt);
    virtual bool handleEvent(const sf::Event& event);

private:
    // NUI
    nui::uiCore m_uiCore;

    // Dungeon
    dungeon::Data m_dungeonData;
    dungeon::Inter m_dungeonInter;
};
