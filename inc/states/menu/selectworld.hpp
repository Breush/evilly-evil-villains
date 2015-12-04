#pragma once

#include "states/state.hpp"
#include "states/identifiers.hpp"
#include "nui/list.hpp"
#include "nui/hstacker.hpp"
#include "nui/button.hpp"
#include "scene/wrappers/rectanglegradient.hpp"
#include "scene/wrappers/label.hpp"

#include <array>

namespace states
{
    class MenuSelectWorld final : public State
    {
        using baseClass = State;

    public:

        //! Constructor.
        MenuSelectWorld(StateStack& stack);

        //! Default destructor.
        ~MenuSelectWorld() = default;

        //----------------------//
        //! @name State control
        //! @{

        StateID id() const noexcept final { return StateID::MENU_SELECTWORLD; }

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void handleEvent(const sf::Event& event) final;

        //! @}


        //---------------//
        //! @name Action
        //! @{

        //! Start game on selected world.
        void playOnSelectedWorld();

        //! @}

    private:

        // NUI
        nui::List m_list;                       //!< List of worlds.
        nui::HStacker m_stacker;                //!< Stacker for buttons.
        std::array<nui::Button, 3u> m_buttons;  //!< Buttons Create/Back/Play.

        // Decorum
        scene::Label m_title;                   //!< Title.
        scene::RectangleGradient m_background;  //!< Background.
    };
}
