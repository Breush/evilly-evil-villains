#pragma once

#include "states/state.hpp"
#include "states/identifiers.hpp"
#include "nui/vstacker.hpp"
#include "nui/button.hpp"
#include "nui/form.hpp"
#include "nui/textentry.hpp"
#include "nui/popdialog.hpp"
#include "sfe/label.hpp"
#include "sfe/rectangleshape.hpp"

#include <array>

namespace states
{
    //! The menu state to create a villain.

    class MenuCreateVillain final : public State
    {
        using baseClass = State;

    public:

        //! Constructor.
        MenuCreateVillain(StateStack& stack);

        //! Default destructor.
        ~MenuCreateVillain() = default;

        //! The state identifier.
        StateID id() const noexcept final { return StateID::MENU_CREATEVILLAIN; }

        //---------------//
        //! @name Events
        //! @{

        bool handleEvent(const sf::Event& event) final;

        //! @}

    protected:

        //---------------------------//
        //! @name Villain management
        //! @{

        //! Create the villain with the current information.
        void createVillain();

        //! @}

    private:

        // NUI
        nui::Form m_form;                       //!< The form.
        nui::VStacker m_stacker;                //!< The buttons stacker.
        nui::TextEntry m_villainNameEntry;      //!< The entry for villain name.
        nui::PopDialog m_popDialog;             //!< To inform about errors.
        std::array<nui::Button, 2> m_buttons;   //!< Create / Back

        // Background and title
        sfe::Label m_title;                 //!< The title of the menu page.
        sfe::RectangleShape m_background;   //!< The background of the menu page.
    };
}
