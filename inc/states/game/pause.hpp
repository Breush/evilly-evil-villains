#pragma once

#include "states/state.hpp"
#include "states/identifiers.hpp"
#include "nui/button.hpp"
#include "sfe/rectangleshape.hpp"
#include "sfe/label.hpp"

class GamePauseState : public State
{
    using baseClass = State;

public:
    GamePauseState(StateStack& stack);
    virtual ~GamePauseState() = default;
    StateID id() const noexcept override { return StateID::GAME_PAUSE; }

    // Routines
    bool handleEvent(const sf::Event& event) override;

private:
    // Decorum
    sfe::RectangleShape m_background;
    sfe::Label m_pauseText;

    // NUI
    nui::Button m_continueButton;
    nui::Button m_mainMenuButton;
};
