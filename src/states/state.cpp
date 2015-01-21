#include "states/state.hpp"
#include "states/statestack.hpp"

State::State(StateStack& stack)
    : mStack(&stack)
{
}

State::~State()
{
}

void State::stackPopPush(States::ID stateID)
{
    mStack->popState();
    mStack->pushState(stateID);
}

void State::stackPush(States::ID stateID)
{
    mStack->pushState(stateID);
}

void State::stackPop()
{
    mStack->popState();
}

void State::stackClear()
{
    mStack->clearStates();
}
