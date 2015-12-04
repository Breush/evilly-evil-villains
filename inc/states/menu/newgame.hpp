#pragma once

#include "states/state.hpp"
#include "states/identifiers.hpp"
#include "scene/wrappers/rectanglegradient.hpp"
#include "scene/wrappers/label.hpp"
#include "nui/form.hpp"
#include "nui/scrollarea.hpp"
#include "nui/choicebox.hpp"
#include "nui/hstacker.hpp"
#include "nui/button.hpp"
#include "dungeon/villaindisplay.hpp"

#include <array>

namespace states
{
    //! State to start a new game.

    class MenuNewGame final : public State
    {
        using baseClass = State;

    public:

        //! Constructor.
        MenuNewGame(StateStack& stack);

        //! Destructor.
        ~MenuNewGame();

        StateID id() const noexcept final { return StateID::MENU_NEWGAME; }

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onShow() noexcept final;
        void handleEvent(const sf::Event& event) final;

        //! @}

        //------------------------//
        //! @name Game management
        //! @{

        //! Start playing on the world.
        void startPlay();

        //! Callback whenever a villain display is clicked.
        void onVillainClicked(dungeon::VillainDisplay& clickedEntity);

        //! @}

        //------------//
        //! @name ICU
        //! @{

        //! Refresh the villains list.
        void refreshVillainsList();

        //! @}

    private:

        dungeon::VillainDisplay* m_selectedVillainDisplay = nullptr;    //!< Currently selected display.

        // Background and title
        scene::Label m_title;                   //!< The title.
        scene::RectangleGradient m_background;  //!< The background.

        // NUI
        nui::ScrollArea m_scrollArea;                               //!< The scroll area.
        nui::HStacker m_globalStacker;                              //!< Stacking the content inside the scroll area.
        std::vector<dungeon::VillainDisplayPtr> m_villainsDisplays; //!< The villains shown.

        nui::HStacker m_buttonsStacker;         //!< The stacker for buttons.
        std::array<nui::Button, 2u> m_buttons;  //!< The buttons (cancel/ok).
    };
}
