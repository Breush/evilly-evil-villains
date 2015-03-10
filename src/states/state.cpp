#include "states/state.hpp"

#include "core/application.hpp"
#include "states/statestack.hpp"

using namespace states;

State::State(StateStack& stack)
    : m_stack(&stack)
{
}

//----- Scene graph calls -----//

// TODO The target should be passed here
void State::draw()
{
    auto& window = Application::context().window;
    window.draw(m_sceneGraph);
}

bool State::update(const sf::Time& dt)
{
    m_sceneGraph.update(dt);
    return false;
}

bool State::handleEvent(const sf::Event& event)
{
    m_sceneGraph.handleEvent(event);
    return false;
}

//----- Stack operations -----//

void State::stackPopPush(StateID stateID)
{
    m_stack->popState();
    m_stack->pushState(stateID);
}

void State::stackPush(StateID stateID)
{
    m_stack->pushState(stateID);
}

void State::stackPop()
{
    m_stack->popState();
}

void State::stackClear()
{
    m_stack->clearStates();
}

void State::stackClear(StateID stateID)
{
    m_stack->clearStates();
    m_stack->pushState(stateID);
}
