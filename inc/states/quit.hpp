#pragma once

#include "states/state.hpp"
#include "nui/button.hpp"
#include "sfe/rectangleshape.hpp"
#include "sfe/label.hpp"

class QuitState : public State
{
public:
    QuitState(StateStack& stack);
    virtual ~QuitState() {}
    virtual States::ID id() const override { return States::QUIT; }

    // Routines
    bool handleEvent(const sf::Event& event) override;

private:
    // Decorum
    sfe::RectangleShape m_background;
    sfe::Label m_question;

    // NUI
    nui::Button m_noButton;
    nui::Button m_yesButton;
};

