#pragma once

#include "states/identifiers.hpp"
#include "resources/identifiers.hpp"

#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>

#include <memory>

namespace sf
{
    class RenderWindow;
}

class StateStack;
class MusicPlayer;
class SoundPlayer;

class State
{
public:
    State(StateStack& stack);
    virtual	~State() {}
    virtual States::ID id() const { return States::NONE; }

    virtual void draw() = 0;
    virtual bool update(const sf::Time& dt) = 0;
    virtual bool handleEvent(const sf::Event& event) = 0;

    // Called whenever display changes
    virtual void onShow() { refresh(); }
    virtual void onHide() { refresh(); }
    virtual void onQuit() {}
    virtual void refresh() {}

protected:
    void stackPopPush(States::ID stateID);
    void stackPush(States::ID stateID);
    void stackPop();
    void stackClear();
    void stackClear(States::ID stateID);

private:
    StateStack*	m_stack;
};
