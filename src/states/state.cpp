#include "states/state.hpp"

#include "core/application.hpp"
#include "states/statestack.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

using namespace states;

State::State(StateStack& stack)
    : m_stack(stack)
{
    // Refresh graph (init size, etc.)
    m_sceneGraph.refreshWindow(Application::context().windowInfo);
    m_sceneGraph.refreshNUI(Application::context().nuiGuides);
}

//-------------------//
//----- Routine -----//

void State::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(m_sceneGraph, states);
}

bool State::update(const sf::Time& dt)
{
    m_sceneGraph.update(dt);
    return true;
}

void State::handleEvent(const sf::Event& event)
{
    m_sceneGraph.handleEvent(event);
}

//----------------------------//
//----- Stack operations -----//

void State::stackReplace(StateID stateID)
{
    m_stack.replaceState(*this, stateID);
}

void State::stackPopPush(StateID stateID)
{
    m_stack.popState();
    m_stack.pushState(stateID);
}

void State::stackPush(StateID stateID)
{
    m_stack.pushState(stateID);
}

void State::stackPop()
{
    m_stack.popState();
}

void State::stackClear()
{
    m_stack.clearStates();
}

void State::stackClear(StateID stateID)
{
    m_stack.clearStates();
    m_stack.pushState(stateID);
}

bool State::isStateVisible(StateID stateID) const
{
    return m_stack.isStateVisible(stateID);
}
