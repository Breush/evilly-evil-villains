#pragma once

#include "states/state.hpp"
#include "states/identifiers.hpp"
#include "nui/pushbutton.hpp"
#include "nui/vstacker.hpp"
#include "scene/wrappers/rectangleshape.hpp"
#include "scene/wrappers/label.hpp"

namespace states
{
    class GamePause final : public State
    {
        using baseClass = State;

    public:

        //! Constructor.
        GamePause(StateStack& stack);

        //! Default destructor.
        ~GamePause();

        //----------------------//
        //! @name State control
        //! @{

        StateID id() const noexcept final { return StateID::GAME_PAUSE; }

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        bool update(const sf::Time& dt) final;
        void handleEvent(const sf::Event& event) final;
        void refreshWindow(const config::WindowInfo& cWindow) final;

        //! @}

    private:

        // Decorum
        scene::RectangleShape m_background;   //!< Background.
        scene::Label m_title;                 //!< Title.

        // NUI
        nui::VStacker m_stacker;        //!< Stacker for the buttons.
        nui::PushButton m_continueButton;   //!< Continue button.
        nui::PushButton m_configButton;     //!< Config button.
        nui::PushButton m_mainMenuButton;   //!< Go back to main menu.
    };
}
