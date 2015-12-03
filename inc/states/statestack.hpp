#pragma once

#include "states/identifiers.hpp"
#include "config/windowinfo.hpp"
#include "config/nuiguides.hpp"
#include "tools/platform-fixes.hpp"

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
    // Forward declarations

    class State;

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

        // Note these kind of functions could replace pushState,
        // but this removes StateID utility. Is it really better?
        //! Dynamically push a state to the front.
        template <class State_t, typename... Args>
        inline void dynamicPushState(Args&&... args)
        {
            PendingChange change;
            change.action = Action::DYNAMIC_PUSH;
            change.pointer = std::make_unique<State_t>(*this, std::forward<Args>(args)...);
            m_pendingChanges.emplace_back(std::move(change));
        }

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
            m_factories[stateID] = [this] () -> std::unique_ptr<State>
                { return std::make_unique<T>(*this); };
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
            DYNAMIC_PUSH,   //!< Add a state to the top, only used via dynamic functions.
            PUSH,           //!< Add a state on top.
            POP,            //!< Remove the state on top.
            CLEAR,          //!< Remove all states.
            REPLACE,        //!< Replace a state with another one.
        };

        //! An action to do on a state.
        struct PendingChange
        {
            Action action;                  //!< The action.
            StateID stateID;                //!< The state.
            std::shared_ptr<State> pointer; //!< The state to create if DYNAMIC_PUSH action.
            const State* pState = nullptr;  //!< The state reference.
        };

    private:

        //! The stack consists in a stack of states.
        std::vector<std::shared_ptr<State>> m_stack;

        //! The pending changes list.
        std::vector<PendingChange> m_pendingChanges;

        //! Creates states according to their ID.
        std::map<StateID, std::function<std::unique_ptr<State>()>> m_factories;
    };
}
