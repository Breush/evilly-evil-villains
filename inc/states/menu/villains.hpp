#pragma once

#include "states/state.hpp"
#include "states/identifiers.hpp"
#include "nui/button.hpp"
#include "nui/list.hpp"
#include "sfe/label.hpp"
#include "sfe/rectangleshape.hpp"

namespace states
{
    //! The menu state to manage villains.

    class MenuVillains final : public State
    {
        using baseClass = State;

    public:

        //! Constructor.
        MenuVillains(StateStack& stack);

        //! Default destructor.
        ~MenuVillains() = default;

        //! The state identifier.
        StateID id() const noexcept final { return StateID::MENU_VILLAINS; }

        //--------------//
        //! @name Events
        //! @{

        bool handleEvent(const sf::Event& event) final;

        //! @}

    private:

        // NUI
        nui::List m_list;           //!< The list of villains.
        nui::Button m_backButton;   //!< The button to go back to previous screen.

        // Background and title
        sfe::Label m_title;                 //!< The title of the menu page.
        sfe::RectangleShape m_background;   //!< The background of the menu page.
    };
}
