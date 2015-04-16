#pragma once

#include "states/state.hpp"
#include "states/identifiers.hpp"
#include "dungeon/data.hpp"
#include "dungeon/graph.hpp"
#include "dungeon/inter.hpp"
#include "dungeon/panel.hpp"
#include "dungeon/summary.hpp"
#include "dungeon/sidebar.hpp"
#include "dungeon/lock.hpp"
#include "dungeon/hero.hpp"
#include "sfe/sprite.hpp"

namespace states
{
    class GameDungeonDesign final : public State
    {
    public:
        GameDungeonDesign(StateStack& stack);
        virtual ~GameDungeonDesign() = default;
        StateID id() const noexcept final { return StateID::GAME_DUNGEON_DESIGN; }

        // Routines
        bool handleEvent(const sf::Event& event) final;

        // Virtual
        void onQuit() noexcept final;

    private:
        // Dungeon
        dungeon::Data m_dungeonData;
        dungeon::Graph m_dungeonGraph;
        dungeon::Inter m_dungeonInter;
        dungeon::Panel m_dungeonPanel;
        dungeon::Sidebar m_dungeonSidebar;
        dungeon::Summary m_dungeonSummary;
        dungeon::Lock m_dungeonLock;
        dungeon::Hero m_dungeonHero;

        // NUI
        nui::ContextMenu m_contextMenu;

        // Decorum
        sfe::Sprite m_decorumBack;
        sfe::Sprite m_decorumMiddle;
        sfe::Sprite m_decorumFront;
    };
}
