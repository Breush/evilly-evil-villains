#pragma once

#include "states/state.hpp"
#include "states/identifiers.hpp"
#include "nui/list.hpp"
#include "nui/hstacker.hpp"
#include "nui/button.hpp"
#include "sfe/rectangleshape.hpp"

#include <array>

class MenuSelectWorldState : public State
{
    using baseClass = State;

public:
    MenuSelectWorldState(StateStack& stack);
    virtual ~MenuSelectWorldState() = default;
    StateID id() const noexcept override { return StateID::MENU_SELECTWORLD; }

    // Routines
    bool handleEvent(const sf::Event& event) override;

private:
    // NUI
    nui::List m_list;
    nui::HStacker m_stacker;
    std::array<nui::Button, 3> m_buttons;

    // Background
    sfe::RectangleShape m_background;
};

