#pragma once

#include "states/state.hpp"
#include "states/identifiers.hpp"
#include "nui/button.hpp"
#include "sfe/rectangleshape.hpp"
#include "sfe/label.hpp"

namespace states
{
    class Quit : public State
    {
    public:
        Quit(StateStack& stack);
        virtual ~Quit() = default;
        StateID id() const noexcept override { return StateID::QUIT; }

        // Routines
        bool handleEvent(const sf::Event& event) override;

    private:
        // Decorum
        sfe::RectangleShape m_background;
        sfe::Label m_title;

        // NUI
        nui::Button m_noButton;
        nui::Button m_yesButton;
    };
}

