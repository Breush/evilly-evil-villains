#pragma once

#include "states/state.hpp"
#include "states/identifiers.hpp"
#include "nui/vstacker.hpp"
#include "nui/button.hpp"
#include "nui/list.hpp"
#include "nui/popdialog.hpp"
#include "sfe/label.hpp"
#include "sfe/rectangleshape.hpp"

#include <array>

namespace states
{
    //! The menu state to manage villains.

    class MenuManageVillains final : public State
    {
        using baseClass = State;

    public:

        //! Constructor.
        MenuManageVillains (StateStack& stack);

        //! Default destructor.
        ~MenuManageVillains () = default;

        //----------------------//
        //! @name State control
        //! @{

        //! The state identifier.
        StateID id() const noexcept final { return StateID::MENU_MANAGEVILLAINS; }
        void onShow() noexcept final { refreshVillainsList(); }

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

        //! Remove the selected villain if possible.
        void deleteSelectedVillain();

        //! The deletion was confirmed, this is a callback.
        void confirmDeletion(uint villain);

        //! @}

        //--------------------------------//
        //! @name Internal change updates
        //! @{

        //! Refresh the whole list from current context.
        void refreshVillainsList() noexcept;

        //! @}

    private:

        // NUI
        nui::List m_list;                       //!< The list of villains.
        nui::VStacker m_stacker;                //!< The buttons stacker.
        nui::PopDialog m_popDialog;             //!< To inform about errors.
        std::array<nui::Button, 3> m_buttons;   //!< Create / Delete / Back

        // Background and title
        sfe::Label m_title;                 //!< The title of the menu page.
        sfe::RectangleShape m_background;   //!< The background of the menu page.
    };
}
