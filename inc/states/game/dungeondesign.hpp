#pragma once

#include "states/state.hpp"
#include "states/identifiers.hpp"
#include "dungeon/data.hpp"
#include "dungeon/graph.hpp"
#include "dungeon/inter.hpp"
#include "dungeon/sidebar/sidebar.hpp"
#include "scene/wrappers/rectangleshape.hpp"

#include <SFML/Graphics/RenderTexture.hpp>

namespace states
{
    class GameDungeonDesign final : public State
    {
        using baseClass = State;

    public:

        //! Constructor.
        GameDungeonDesign(StateStack& stack);

        //! Destructor.
        ~GameDungeonDesign();

        //----------------------//
        //! @name State control
        //! @{

        StateID id() const noexcept final { return StateID::GAME_DUNGEON_DESIGN; }
        void onQuit() noexcept final;

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        bool update(const sf::Time& dt) final;
        void handleEvent(const sf::Event& event) final;
        void refreshWindow(const config::WindowInfo& cWindow) final;

        //! @}

        //----------------//
        //! @name Loading
        //! @{

        //! Progressively load resources in memory.
        void updateLoading(const sf::Time& dt);

        //! @}

    private:

        // Dungeon
        dungeon::Data m_dungeonData;
        dungeon::Graph m_dungeonGraph;
        dungeon::Inter m_dungeonInter;
        dungeon::Sidebar m_dungeonSidebar;

        // Loading
        scene::Label m_loadingText;
        scene::RectangleShape m_loadingBackground;
        bool m_loading = true;
        uint m_loadingStep = 0u;
        uint m_loadingPercent = 0u;

        // NUI
        nui::ContextMenu m_contextMenu;

        // Scene
        scene::RectangleShape m_sceneFront;
        scene::RectangleShape m_sceneClose;
        scene::RectangleShape m_sceneMiddle;
        scene::RectangleShape m_sceneFar;
        scene::RectangleShape m_sceneHorizon;
        scene::RectangleShape m_sceneSky;

        const float m_depthFront    = 0.95f;
        const float m_depthClose    = 1.f;
        const float m_depthMiddle   = 1.05f;
        const float m_depthFar      = 1.1f;
        const float m_depthHorizon  = 1.15f;
        const float m_depthSky      = 1.3f;
    };
}
