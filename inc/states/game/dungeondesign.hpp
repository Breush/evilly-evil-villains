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

    void draw() override;
    bool update(const sf::Time& dt) override;
    bool handleEvent(const sf::Event& event) override;

    void onQuit() override;

private:
    // NUI
    nui::uiCore m_uiCore;

    // Dungeon
    dungeon::Data m_dungeonData;
    dungeon::Inter m_dungeonInter;
};
