#pragma once

#include "states/state.hpp"
#include "dungeon/data.hpp"
#include "dungeon/inter.hpp"
#include "dungeon/panel.hpp"
#include "sfe/sprite.hpp"

class GameDungeonDesignState : public State
{
public:
    GameDungeonDesignState(StateStack& stack);
    virtual ~GameDungeonDesignState() {}
    States::ID id() const override { return States::GAME_DUNGEON_DESIGN; }

    // Routines
    bool handleEvent(const sf::Event& event) override;

    // Virtual
    void onQuit() override;

private:
    // Dungeon
    dungeon::Data m_dungeonData;
    dungeon::Inter m_dungeonInter;
    dungeon::Panel m_dungeonPanel;

    // NUI
    nui::ContextMenu m_contextMenu;

    // Decorum
    sfe::Sprite m_decorumBack;
    sfe::Sprite m_decorumFront;
};
