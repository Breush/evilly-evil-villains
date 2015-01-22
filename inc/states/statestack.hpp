#pragma once

#include "states/state.hpp"
#include "states/identifiers.hpp"
#include "resources/identifiers.hpp"

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

class StateStack : private sf::NonCopyable
{
public:
    enum Action {
        Push,
        Pop,
        Clear,
    };

public:
    explicit StateStack();

    template <typename T>
    void registerState(States::ID stateID);

    void update(sf::Time dt);
    void draw();
    void handleEvent(const sf::Event& event);

    void pushState(States::ID stateID);
    void popState();
    void clearStates();

    bool isEmpty() const;

    // Called whenever display changes
    void refresh();

private:
    State::Ptr createState(States::ID stateID);
    void applyPendingChanges();

    struct PendingChange {
        explicit PendingChange(Action action, States::ID stateID = States::NONE);
        Action action;
        States::ID stateID;
    };


private:
    std::vector<State::Ptr> m_stack;
    std::vector<PendingChange> m_pendingList;
    std::map<States::ID, std::function<State::Ptr()>> m_factories;
};


template <typename T>
void StateStack::registerState(States::ID stateID)
{
    m_factories[stateID] = [this]() {
        return State::Ptr(new T(*this));
    };
}
