#pragma once

#include "states/state.hpp"
#include "nui/button.hpp"
#include "sfe/rectangleshape.hpp"
#include "sfe/label.hpp"

class GamePauseState : public State
{
    typedef State baseClass;

public:
    GamePauseState(StateStack& stack);
    virtual ~GamePauseState() {}
    States::ID id() const override { return States::GAME_PAUSE; }

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
