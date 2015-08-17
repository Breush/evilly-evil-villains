#pragma once

#include "states/state.hpp"
#include "states/identifiers.hpp"
#include "dungeon/data.hpp"
#include "dungeon/graph.hpp"
#include "dungeon/inter.hpp"
#include "dungeon/sidebar.hpp"
#include "dungeon/lock.hpp"
#include "dungeon/log.hpp"
#include "dungeon/heroesmanager.hpp"
#include "sfe/rectangleshape.hpp"

namespace states
{
    class GameDungeonDesign final : public State
    {
        using baseClass = State;

    public:

        GameDungeonDesign(StateStack& stack);
        virtual ~GameDungeonDesign() = default;
        StateID id() const noexcept final { return StateID::GAME_DUNGEON_DESIGN; }

        // Routine
        bool update(const sf::Time& dt) final;
        void refreshWindow(const config::WindowInfo& cWindow) final;

        // Events
        bool handleEvent(const sf::Event& event) final;

        // Virtual
        void onQuit() noexcept final;

    private:

        // Dungeon
        dungeon::Data m_dungeonData;
        dungeon::Graph m_dungeonGraph;
        dungeon::Inter m_dungeonInter;
        dungeon::Sidebar m_dungeonSidebar;
        dungeon::Lock m_dungeonLock;
        dungeon::Log m_dungeonLog;
        dungeon::HeroesManager m_heroesManager;

        // NUI
        nui::ContextMenu m_contextMenu;

        // Scene
        sfe::RectangleShape m_sceneFront;
        sfe::RectangleShape m_sceneClose;
        sfe::RectangleShape m_sceneMiddle;
        sfe::RectangleShape m_sceneFar;
        sfe::RectangleShape m_sceneHorizon;
        sfe::RectangleShape m_sceneSky;

        const float m_depthFront    = 0.95f;
        const float m_depthClose    = 1.f;
        const float m_depthMiddle   = 1.05f;
        const float m_depthFar      = 1.1f;
        const float m_depthHorizon  = 1.15f;
        const float m_depthSky      = 1.3f;
    };
}
