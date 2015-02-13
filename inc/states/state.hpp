#pragma once

#include "states/identifiers.hpp"
#include "scene/graph.hpp"
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

    virtual void draw();
    virtual bool update(const sf::Time& dt);
    virtual bool handleEvent(const sf::Event& event);
    // Getter
    scene::Entity& sceneLayer(Layers::ID layerID) { return m_sceneGraph[int(layerID)]; }

    // Called whenever display changes
    virtual void onShow() { m_sceneGraph.updateFocusSprite(); }
    virtual void onHide() {}
    virtual void onQuit() {}

protected:
    void stackPopPush(States::ID stateID);
    void stackPush(States::ID stateID);
    void stackPop();
    void stackClear();
    void stackClear(States::ID stateID);

private:
    StateStack*	m_stack;

    scene::Graph m_sceneGraph;
};
