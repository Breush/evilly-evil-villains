#pragma once

#include "nui/uicore.hpp"
#include "dungeon/data.hpp"
#include "dungeon/inter.hpp"
#include "dungeon/panel.hpp"
#include "states/state.hpp"

#include <SFML/Graphics/Sprite.hpp>

class GameDungeonDesignState : public State
{
public:
    GameDungeonDesignState(StateStack& stack);
    virtual ~GameDungeonDesignState() {}
    States::ID id() const override { return States::GAME_DUNGEON_DESIGN; }

    // Routines
    void draw() override;
    bool update(const sf::Time& dt) override;
    bool handleEvent(const sf::Event& event) override;

    // Virtual
    void onQuit() override;

private:
    // NUI
    interaction::MouseDetector m_mouseDetector;
    nui::uiCore m_uiCore;

    // Dungeon
    dungeon::Data m_dungeonData;
    dungeon::Inter m_dungeonInter;
    dungeon::Panel m_dungeonPanel;

    // Decorum
    sf::Sprite m_decorumBack;
    sf::Sprite m_decorumFront;
};
