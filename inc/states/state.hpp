#pragma once

#include "scene/graph.hpp"
#include "tools/int.hpp"

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>

#include <memory>

// Forward declarations

enum class StateID : uint8;

namespace states
{
    // Forward declarations

    class StateStack;

    //! Abstract state of the state stack machine.

    class State : public sf::Drawable
    {
    public:

        //! Constructor.
        State(StateStack& stack);

        //! Default destructor.
        virtual ~State() = default;

        //----------------//
        //! @name Routine
        //! @{

        //! Draw the state.
        void draw(sf::RenderTarget& target, sf::RenderStates states) const final;

        //! Update the state.
        //! Return false to prevent update of below states in the pile.
        virtual bool update(const sf::Time& dt);

        //! Let the graph handle event.
        virtual void handleEvent(const sf::Event& event);

        //! Reset views to current screen status.
        inline virtual void refreshWindow(const config::WindowInfo& cWindow) { m_sceneGraph.refreshWindow(cWindow); }

        //! Refresh NUI appearance.
        inline virtual void refreshNUI(const config::NUIGuides& cNUI) { m_sceneGraph.refreshNUI(cNUI); }

        //! @}

        //----------------------//
        //! @name State control
        //! @{

        //! ID binding.
        virtual StateID id() const noexcept = 0;

        //! Called whenever the state is shown on top.
        virtual void onShow() noexcept { m_sceneGraph.updateFocusSprite(); }

        //! Called whenever the state is no more on top.
        virtual void onHide() noexcept {}

        //! Called whenever the state is removed.
        virtual void onQuit() noexcept {}

        //! @}

    protected:

        //----------------//
        //! @name Getters
        //! @{

        //! Get the scene from the graph.
        inline scene::Scene& scene() { return m_sceneGraph.scene(); }

        //! Get the NUI layer from the graph.
        inline scene::Layer& nuiLayer() { return m_sceneGraph.nuiLayer(); }

        //! @}

        //-------------------------//
        //! @name Stack operations
        //! @{

        //! Remove top state.
        void stackPop();

        //! Remove all states.
        void stackClear();

        //! Remove all states then add the specified one.
        void stackClear(StateID stateID);

        //! Add the specified state on top.
        void stackPush(StateID stateID);

        //! Remove top state then add the specified state on top.
        void stackPopPush(StateID stateID);

        //! @}

    private:

        StateStack& m_stack;        //!< Referenced stack.
        scene::Graph m_sceneGraph;  //!< State graph.
    };
}
