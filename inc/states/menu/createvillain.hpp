#pragma once

#include "states/state.hpp"
#include "states/identifiers.hpp"
#include "nui/vstacker.hpp"
#include "nui/pushbutton.hpp"
#include "nui/form.hpp"
#include "nui/textentry.hpp"
#include "nui/popdialog.hpp"
#include "scene/wrappers/label.hpp"
#include "scene/wrappers/rectanglegradient.hpp"

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

        //----------------------//
        //! @name State control
        //! @{

        //! The state identifier.
        StateID id() const noexcept final { return StateID::MENU_CREATEVILLAIN; }

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void handleEvent(const sf::Event& event) final;

        //! @}

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
        std::array<nui::PushButton, 2> m_buttons;   //!< Create / Back

        // Background and title
        scene::Label m_title;                   //!< The title of the menu page.
        scene::RectangleGradient m_background;  //!< The background of the menu page.
    };
}
