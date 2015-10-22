#pragma once

#include "states/state.hpp"
#include "states/identifiers.hpp"
#include "scene/wrappers/label.hpp"
#include "scene/wrappers/rectangleshape.hpp"

namespace states
{
    //! The menu state to configure various options.

    class MenuConfig final : public State
    {
        using baseClass = State;

    public:

        //! Constructor.
        MenuConfig(StateStack& stack);

        //! Default destructor.
        ~MenuConfig() = default;

        //----------------------//
        //! @name State control
        //! @{

        StateID id() const noexcept final { return StateID::MENU_CONFIG; }

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void handleEvent(const sf::Event& event) final;

        //! @}

    private:

        // Background and title
        scene::Label m_title;                 //!< The title of the menu page.
        scene::RectangleShape m_background;   //!< The background of the menu page.
    };
}
