#include "states/statestack.hpp"

#include "core/application.hpp"
#include "states/identifiers.hpp"
#include "config/nuiguides.hpp"
#include "tools/platform-fixes.hpp" // reverse
#include "tools/debug.hpp"
#include "tools/tools.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

using namespace states;

//-------------------//
//----- Routine -----//

void StateStack::update(const sf::Time& dt)
{
    // Iterate from top to bottom, stop as soon as update() returns false
    for (auto& state : std::reverse(m_stack))
        if (!state->update(dt))
            break;

    applyPendingChanges();
}

void StateStack::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    // Draw all active states from bottom to top
    for (const auto& state : m_stack)
        target.draw(*state, states);
}

void StateStack::handleEvent(const sf::Event& event)
{
    returnif (m_stack.empty());

    // Front state keeps event control
    m_stack.back()->handleEvent(event);
}

void StateStack::refreshWindow(const config::WindowInfo& cWindow)
{
    for (auto& state : m_stack)
        state->refreshWindow(cWindow);
}

void StateStack::refreshNUI(const config::NUIGuides& cNUI)
{
    for (auto& state : m_stack)
        state->refreshNUI(cNUI);
}

//-----------------//
//----- Stack -----//

bool StateStack::isEmpty() const
{
    return m_stack.empty();
}

bool StateStack::isStateVisible(StateID stateID) const
{
    for (const auto& state : m_stack)
        if (state->id() == stateID)
            return true;

    for (PendingChange change : m_pendingChanges)
        if (change.action == Action::PUSH && change.stateID == stateID)
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
    for (PendingChange change : m_pendingChanges) {
        switch (change.action) {
        case Action::PUSH:
            if (!m_stack.empty()) m_stack.back()->onHide();
            m_stack.emplace_back(std::move(createState(change.stateID)));
            m_stack.back()->refreshWindow(Application::context().windowInfo);
            m_stack.back()->refreshNUI(Application::context().nuiGuides);
            break;

        case Action::POP:
            if (!m_stack.empty()) m_stack.back()->onQuit();
            m_stack.pop_back();
            if (!m_stack.empty()) m_stack.back()->onShow();
            break;

        case Action::CLEAR:
            for (auto& state : m_stack) state->onQuit();
            m_stack.clear();
            break;

        case Action::REPLACE:
            for (auto& state : m_stack)
            if (change.pState == state.get()) {
                state->onQuit();
                state = std::move(createState(change.stateID));
                state->refreshWindow(Application::context().windowInfo);
                state->refreshNUI(Application::context().nuiGuides);
                break;
            }
            break;
        }
    }

    // Clear list
    m_pendingChanges.clear();
}

//--------------------------//
//----- States control -----//

void StateStack::replaceState(const State& state, StateID stateID)
{
    PendingChange change;
    change.action = Action::REPLACE;
    change.stateID = stateID;
    change.pState = &state;
    m_pendingChanges.emplace_back(std::move(change));
}

void StateStack::pushState(StateID stateID)
{
    PendingChange change;
    change.action = Action::PUSH;
    change.stateID = stateID;
    m_pendingChanges.emplace_back(std::move(change));
}

void StateStack::popState()
{
    PendingChange change;
    change.action = Action::POP;
    change.stateID = StateID::NONE;
    m_pendingChanges.emplace_back(std::move(change));
}

void StateStack::clearStates()
{
    PendingChange change;
    change.action = Action::CLEAR;
    change.stateID = StateID::NONE;
    m_pendingChanges.emplace_back(std::move(change));
}
