#pragma once

#include "states/state.hpp"
#include "states/identifiers.hpp"
#include "sfe/rectangleshape.hpp"
#include "sfe/label.hpp"
#include "nui/textentry.hpp"
#include "nui/hstacker.hpp"

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

    // NUI
    nui::HStacker m_stacker;
    sfe::Label m_worldNameLabel;
    nui::TextEntry m_worldNameEntry;
};

