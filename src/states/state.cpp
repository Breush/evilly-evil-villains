#include "states/state.hpp"
#include "states/statestack.hpp"


State::Context::Context(sf::RenderWindow& pwindow, TextureHolder& ptextures, ShaderHolder& pshaders,
                        FontHolder& pfonts, MusicPlayer& pMusic, SoundPlayer& pSounds)
    : window(&pwindow)
    , textures(&ptextures)
    , shaders(&pshaders)
    , fonts(&pfonts)
    , music(&pMusic)
    , sounds(&pSounds)
{
}

State::State(StateStack& stack, Context context)
    : mStack(&stack)
    , mContext(context)
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

State::Context State::getContext() const
{
    return mContext;
}
