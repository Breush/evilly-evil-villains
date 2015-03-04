#pragma once

#include "states/state.hpp"
#include "states/identifiers.hpp"
#include "sfe/rectangleshape.hpp"

class MenuCreateWorldState : public State
{
    using baseClass = State;

public:
    MenuCreateWorldState(StateStack& stack);
    virtual ~MenuCreateWorldState() = default;
    StateID id() const noexcept override { return StateID::MENU_CREATEWORLD; }

    // Routines
    bool handleEvent(const sf::Event& event) override;

private:

    // Background
    sfe::RectangleShape m_background;
};

