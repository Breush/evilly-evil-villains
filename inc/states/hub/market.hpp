#pragma once

#include "states/state.hpp"
#include "states/identifiers.hpp"
#include "nui/scrollarea.hpp"
#include "nui/hstacker.hpp"
#include "nui/vstacker.hpp"
#include "nui/pushbutton.hpp"
#include "scene/wrappers/rectanglegradient.hpp"
#include "scene/wrappers/label.hpp"
#include "hub/traplocker.hpp"

#include <array>

// Forward declarations

namespace dungeon
{
    class Data;
}

namespace states
{
    //! Where to unlock traps in the hub.

    class HubMarket final : public State
    {
        using baseClass = State;

    public:

        //! Constructor.
        HubMarket(StateStack& stack, dungeon::Data& data);

        //! Default destructor.
        ~HubMarket() = default;

        //----------------------//
        //! @name State control
        //! @{

        StateID id() const noexcept final { return StateID::HUB_MARKET; }

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void handleEvent(const sf::Event& event) final;

        //! @}

        //------------//
        //! @name ICU
        //! @{

        void refreshColumns();

        //! @}

    private:

        dungeon::Data& m_data;      //!< Current dungeon data reference.

        // NUI
        nui::ScrollArea m_scrollArea;               //!< The scroll area.
        nui::HStacker m_globalStacker;              //!< Stacker inside the scrollarea.
        std::array<nui::VStacker, 2u> m_columns;    //!< The stackers forming the columns.
        nui::PushButton m_button;                       //!< Back button.

        // Lockers
        std::vector<std::unique_ptr<hub::TrapLocker>> m_trapLockers;    //!< Traps list.

        // Decorum
        scene::Label m_title;                   //!< Title.
        scene::RectangleGradient m_background;  //!< Background.
    };
}
