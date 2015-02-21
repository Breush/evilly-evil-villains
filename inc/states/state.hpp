#pragma once

#include "scene/graph.hpp"
#include "tools/int.hpp"

#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>

#include <memory>

// Forward declarations
enum class StateID : uint8;
class StateStack;

class State
{
public:
    State(StateStack& stack);
    virtual	~State() = default;
    virtual StateID id() const noexcept = 0;

    virtual void draw();
    virtual bool update(const sf::Time& dt);
    virtual bool handleEvent(const sf::Event& event);

    // Getter
    scene::Entity& sceneLayer(LayerID::type layerID) { return m_sceneGraph[layerID]; }

    // Called whenever display changes
    virtual void onShow() noexcept { m_sceneGraph.updateFocusSprite(); }
    virtual void onHide() noexcept {}
    virtual void onQuit() noexcept {}

protected:
    void stackPopPush(StateID stateID);
    void stackPush(StateID stateID);
    void stackPop();
    void stackClear();
    void stackClear(StateID stateID);

private:
    StateStack*	m_stack;

    scene::Graph m_sceneGraph;
};
