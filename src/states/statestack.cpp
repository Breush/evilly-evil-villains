#include "states/statestack.hpp"
#include "tools/debug.hpp"

StateStack::StateStack()
    : m_stack()
    , m_pendingList()
    , m_factories()
{
}

void StateStack::update(sf::Time dt)
{
    // Iterate from top to bottom, stop as soon as update() returns false
    for (auto itr = m_stack.rbegin(); itr != m_stack.rend(); ++itr) {
        if (!(*itr)->update(dt)) {
            break;
        }
    }

    applyPendingChanges();
}

void StateStack::draw()
{
    // Draw all active states from bottom to top
    for (State::Ptr& state : m_stack) {
        state->draw();
    }
}

void StateStack::handleEvent(const sf::Event& event)
{
    // Iterate from top to bottom, stop as soon as handleEvent() returns false
    for (auto itr = m_stack.rbegin(); itr != m_stack.rend(); ++itr) {
        if (!(*itr)->handleEvent(event)) {
            break;
        }
    }

    applyPendingChanges();
}

void StateStack::pushState(States::ID stateID)
{
    m_pendingList.push_back(PendingChange(Push, stateID));
}

void StateStack::popState()
{
    m_pendingList.push_back(PendingChange(Pop));
}

void StateStack::clearStates()
{
    m_pendingList.push_back(PendingChange(Clear));
}

bool StateStack::isEmpty() const
{
    return m_stack.empty();
}

State::Ptr StateStack::createState(States::ID stateID)
{
    auto found = m_factories.find(stateID);
    massert(found != m_factories.end(), "Cannot find state " << stateID);

    return found->second();
}

void StateStack::applyPendingChanges()
{
    // Apply changes
    for (PendingChange change : m_pendingList) {
        switch (change.action) {
        case Push:
            if (!m_stack.empty())
                m_stack.back()->onHide();
            m_stack.push_back(createState(change.stateID));
            break;

        case Pop:
            m_stack.pop_back();
            if (!m_stack.empty())
                m_stack.back()->onShow();
            break;

        case Clear:
            m_stack.clear();
            break;

        default:
            // TODO Gestion des erreurs
            break;
        }
    }

    // Clear list
    m_pendingList.clear();
}

StateStack::PendingChange::PendingChange(Action paction, States::ID pstateID)
    : action(paction)
    , stateID(pstateID)
{
}
