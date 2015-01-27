#include "states/state.hpp"
#include "states/statestack.hpp"

State::State(StateStack& stack)
    : m_stack(&stack)
{
}

//----- Stack operations -----//

void State::stackPopPush(States::ID stateID)
{
    m_stack->popState();
    m_stack->pushState(stateID);
}

void State::stackPush(States::ID stateID)
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

void State::stackClear(States::ID stateID)
{
    m_stack->clearStates();
    m_stack->pushState(stateID);
}
