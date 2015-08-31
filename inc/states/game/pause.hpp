#pragma once

#include "states/state.hpp"
#include "states/identifiers.hpp"
#include "nui/button.hpp"
#include "sfe/rectangleshape.hpp"
#include "sfe/label.hpp"

namespace states
{
    class GamePause final : public State
    {
        using baseClass = State;

    public:

        //! Constructor.
        GamePause(StateStack& stack);

        //! Default destructor.
        ~GamePause() = default;

        //----------------------//
        //! @name State control
        //! @{

        StateID id() const noexcept final { return StateID::GAME_PAUSE; }

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
        nui::Button m_continueButton;   //!< Continue button.
        nui::Button m_mainMenuButton;   //!< Go back to main menu.
    };
}
