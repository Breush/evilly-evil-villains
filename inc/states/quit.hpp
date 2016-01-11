#pragma once

#include "states/state.hpp"
#include "states/identifiers.hpp"
#include "nui/pushbutton.hpp"
#include "scene/wrappers/rectangleshape.hpp"
#include "scene/wrappers/label.hpp"

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
        scene::RectangleShape m_background;   //!< Background.
        scene::Label m_title;                 //!< Title.

        // NUI
        nui::PushButton m_noButton;             //!< Cancel button.
        nui::PushButton m_yesButton;            //!< Accept button.
    };
}

