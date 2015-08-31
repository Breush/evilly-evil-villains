#pragma once

#include "states/state.hpp"
#include "states/identifiers.hpp"
#include "nui/button.hpp"
#include "sfe/rectangleshape.hpp"
#include "sfe/label.hpp"

namespace states
{
    //! State shown when the user wants to quit the game.

    class Quit final : public State
    {
    public:

        //! Constructor.
        Quit(StateStack& stack);

        //! Default destructor.
        ~Quit() = default;

        //----------------------//
        //! @name State control
        //! @{

        StateID id() const noexcept final { return StateID::QUIT; }

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void handleEvent(const sf::Event& event) final;

        //! @}

    private:

        // Decorum
        sfe::RectangleShape m_background;   //!< Background.
        sfe::Label m_title;                 //!< Title.

        // NUI
        nui::Button m_noButton;             //!< Cancel button.
        nui::Button m_yesButton;            //!< Accept button.
    };
}

