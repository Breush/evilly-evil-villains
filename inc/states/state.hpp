#pragma once

#include "scene/graph.hpp"
#include "tools/int.hpp"

#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>

#include <memory>

// Forward declarations

enum class StateID : uint8;

namespace states
{
    // Forward declarations

    class StateStack;

    class State
    {
    public:
        State(StateStack& stack);
        virtual ~State() = default;
        virtual StateID id() const noexcept = 0;

        virtual void draw();
        virtual bool update(const sf::Time& dt);
        virtual bool handleEvent(const sf::Event& event);

        // Getter
        inline scene::Scene& scene() { return m_sceneGraph.scene(); }
        inline scene::Layer& nuiLayer() { return m_sceneGraph.nuiLayer(); }

        //! Reset the graph's views to current screen status.
        inline virtual void refreshDisplay() { m_sceneGraph.refreshDisplay(); }

        // Called whenever display changes
        virtual void onShow() noexcept { m_sceneGraph.updateFocusSprite(); }
        virtual void onHide() noexcept {}
        virtual void onQuit() noexcept {}

    protected:
        void stackPopPush(StateID stateID);
        void stackPush(StateID stateID);
        void stackPop();
        void stackClear();
        void stackClear(StateID stateID);

    private:
        StateStack* m_stack;

        scene::Graph m_sceneGraph;
    };
}
