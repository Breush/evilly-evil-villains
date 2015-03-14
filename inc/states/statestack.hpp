#pragma once

#include "states/state.hpp"

#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Time.hpp>

#include <vector>
#include <utility>
#include <functional>
#include <map>

namespace sf
{
    class Event;
    class RenderWindow;
}

namespace states
{
    class StateStack : private sf::NonCopyable
    {
    public:
        enum class Action {
            PUSH,
            POP,
            CLEAR,
        };

    public:
        explicit StateStack() = default;

        template <typename T>
        void registerState(StateID stateID)
        {
            m_factories[stateID] = [this]() {
                return std::unique_ptr<State>(new T(*this));
            };
        }

        void update(const sf::Time& dt);
        void draw();
        void handleEvent(const sf::Event& event);

        void pushState(StateID stateID);
        void popState();
        void clearStates();

        bool isEmpty() const;
        bool isStateVisible(StateID stateID) const;

        //! Reset the states' views to current screen status.
        void refreshDisplay();

    private:
        std::unique_ptr<State> createState(StateID stateID);
        void applyPendingChanges();

        struct PendingChange {
            explicit PendingChange(Action inAction, StateID inStateID) : action(inAction), stateID(inStateID) {}

            Action action;
            StateID stateID;
        };

    private:
        std::vector<std::unique_ptr<State>> m_stack;
        std::vector<PendingChange> m_pendingList;
        std::map<StateID, std::function<std::unique_ptr<State>()>> m_factories;
    };
}
