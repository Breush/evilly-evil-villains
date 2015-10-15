#pragma once

#include "states/state.hpp"

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Time.hpp>

#include <vector>
#include <utility>
#include <functional>
#include <map>

// Forward declarations

namespace sf
{
    class Event;
    class RenderWindow;
}

namespace states
{
    //! States manager as a stacked states machine.

    class StateStack final : public sf::Drawable, private sf::NonCopyable
    {
    public:

        //! Default constructor.
        StateStack() = default;

        //! Default destructor.
        ~StateStack() = default;

        //----------------//
        //! @name Routine
        //! @{

        //! Draw the stack.
        void draw(sf::RenderTarget& target, sf::RenderStates states) const final;

        //! Update the states.
        void update(const sf::Time& dt);

        //! Handle event.
        void handleEvent(const sf::Event& event);

        //! Reset the states' views to current screen status.
        void refreshWindow(const config::WindowInfo& cWindow);

        //! Refresh states' NUI appearance.
        void refreshNUI(const config::NUIGuides& cNUI);

        //! @}

        //--------------//
        //! @name Stack
        //! @{

        //! Return true is stack has no state active in it.
        bool isEmpty() const;

        //! Return true is specified state exists in stack.
        bool isStateVisible(StateID stateID) const;

        //! @}

        //----------------------//
        //! @name State control
        //! @{

        //! Push a state to the front.
        void pushState(StateID stateID);

        //! Pop front-most state.
        void popState();

        //! Replace the specified state with a new one.
        void replaceState(const State& state, StateID stateID);

        //! Pops all states.
        void clearStates();

        //! Add a new state to the factory.
        template <typename T>
        void registerState(StateID stateID)
        {
            m_factories[stateID] = [this]() {
                return std::unique_ptr<State>(new T(*this));
            };
        }

        //! @}

    protected:

        //--------------//
        //! @name Stack
        //! @{

        //! Create a state from its ID.
        std::unique_ptr<State> createState(StateID stateID);

        //! Apply all pending changes.
        void applyPendingChanges();

        //! @}

        //! Possible actions to do with states.
        enum class Action
        {
            PUSH,       //!< Add a state on top.
            POP,        //!< Remove the state on top.
            CLEAR,      //!< Remove all states.
            REPLACE,    //!< Replace a state with another one.
        };

        //! An action to do on a state.
        struct PendingChange
        {
            Action action;                  //!< The action.
            StateID stateID;                //!< The new state.
            const State* pState = nullptr;  //!< The state reference.
        };

    private:

        //! The stack consists in a stack of states.
        std::vector<std::unique_ptr<State>> m_stack;

        //! The pending changes list.
        std::vector<PendingChange> m_pendingChanges;

        //! Creates states according to their ID.
        std::map<StateID, std::function<std::unique_ptr<State>()>> m_factories;
    };
}
