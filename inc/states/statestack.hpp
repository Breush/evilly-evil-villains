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
    bool isStateVisible(States::ID stateID) const;

    // Called whenever display changes
    void refresh();

private:
    std::unique_ptr<State> createState(States::ID stateID);
    void applyPendingChanges();

    struct PendingChange {
        explicit PendingChange(Action inAction, States::ID inStateID = States::NONE)
            : action(inAction), stateID(inStateID) {}
        Action action;
        States::ID stateID;
    };

private:
    std::vector<std::unique_ptr<State>> m_stack;
    std::vector<PendingChange> m_pendingList;
    std::map<States::ID, std::function<std::unique_ptr<State>()>> m_factories;
};


template <typename T>
void StateStack::registerState(States::ID stateID)
{
    m_factories[stateID] = [this]() {
        return std::unique_ptr<State>(new T(*this));
    };
}
