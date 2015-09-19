#pragma once

#include "states/state.hpp"
#include "states/identifiers.hpp"
#include "scene/wrappers/rectangleshape.hpp"
#include "scene/wrappers/label.hpp"
#include "nui/form.hpp"
#include "nui/textentry.hpp"
#include "nui/choicebox.hpp"
#include "nui/hstacker.hpp"
#include "nui/button.hpp"

#include <array>

namespace states
{
    /*! State for create world menu.
     *  Called by select world menu state.
     */

    class MenuCreateWorld final : public State
    {
        using baseClass = State;

    public:

        //! Constructor.
        MenuCreateWorld(StateStack& stack);

        //! Default destructor.
        ~MenuCreateWorld() = default;

        //----------------------//
        //! @name State control
        //! @{

        StateID id() const noexcept final { return StateID::MENU_CREATEWORLD; }

        void onShow() noexcept final { refreshVillainsList(); baseClass::onShow(); }

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        bool update(const sf::Time& dt) final;
        void handleEvent(const sf::Event& event) final;

        //! @}

        //-------------------------//
        //! @name World management
        //! @{

        //! Create and start playing on the world.
        void createAndPlayWorld();

        //! @}

        //--------------------------------//
        //! @name Internal changes update
        //! @{

        //! Refresh villains list from context::villains.
        void refreshVillainsList();

        //! @}

    private:

        // Background and title
        scene::Label m_title;                 //!< The title.
        scene::RectangleShape m_background;   //!< The background.

        // Form
        nui::Form m_form;                   //!< The form containing the following items.
        nui::TextEntry m_worldNameEntry;    //!< World name entry.
        nui::ChoiceBox m_villainBox;        //!< Villains choice box.
        nui::Button m_createVillainButton;  //!< Create villains state launch.

        // Buttons
        nui::HStacker m_buttonsStacker;         //!< The stacker for buttons.
        std::array<nui::Button, 2> m_buttons;   //!< The buttons (cancel/quit/ok).
    };
}
