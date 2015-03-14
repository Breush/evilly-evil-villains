#include "states/statestack.hpp"

#include "states/identifiers.hpp"
#include "tools/debug.hpp"

using namespace states;

void StateStack::update(const sf::Time& dt)
{
    // Iterate from top to bottom, stop as soon as update() returns false
    for (auto itr = m_stack.rbegin(); itr != m_stack.rend(); ++itr)
        if (!(*itr)->update(dt))
            break;

    applyPendingChanges();
}

void StateStack::draw()
{
    // Draw all active states from bottom to top
    for (auto& state : m_stack)
        state->draw();
}

void StateStack::handleEvent(const sf::Event& event)
{
    // Iterate from top to bottom, stop as soon as handleEvent() returns false
    for (auto itr = m_stack.rbegin(); itr != m_stack.rend(); ++itr)
        if (!(*itr)->handleEvent(event))
            break;

    applyPendingChanges();
}

void StateStack::refreshDisplay()
{
    for (auto& state : m_stack)
        state->refreshDisplay();
}

void StateStack::pushState(StateID stateID)
{
    m_pendingList.emplace_back(Action::PUSH, stateID);
}

void StateStack::popState()
{
    m_pendingList.emplace_back(Action::POP, StateID::NONE);
}

void StateStack::clearStates()
{
    m_pendingList.emplace_back(Action::CLEAR, StateID::NONE);
}

bool StateStack::isEmpty() const
{
    return m_stack.empty();
}

bool StateStack::isStateVisible(StateID stateID) const
{
    for (const auto& state : m_stack)
        if (state->id() == stateID)
            return true;

    return false;
}

std::unique_ptr<State> StateStack::createState(StateID stateID)
{
    auto found = m_factories.find(stateID);
    massert(found != m_factories.end(), "Cannot find state " << static_cast<uint8>(stateID));

    return found->second();
}

void StateStack::applyPendingChanges()
{
    // Apply changes
    for (PendingChange change : m_pendingList) {
        switch (change.action) {
        case Action::PUSH:
            if (!m_stack.empty())
                m_stack.back()->onHide();
            m_stack.emplace_back(std::move(createState(change.stateID)));
            break;

        case Action::POP:
            if (!m_stack.empty())
                m_stack.back()->onQuit();
            m_stack.pop_back();
            if (!m_stack.empty())
                m_stack.back()->onShow();
            break;

        case Action::CLEAR:
            for (auto& state : m_stack)
                state->onQuit();
            m_stack.clear();
            break;

        default:
            break;
        }
    }

    // Clear list
    m_pendingList.clear();
}
