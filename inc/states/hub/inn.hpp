#pragma once

#include "states/state.hpp"
#include "states/identifiers.hpp"
#include "nui/scrollarea.hpp"
#include "nui/hstacker.hpp"
#include "nui/vstacker.hpp"
#include "nui/pushbutton.hpp"
#include "hub/monsterlocker.hpp"
#include "scene/wrappers/rectanglegradient.hpp"
#include "scene/wrappers/label.hpp"

#include <array>

// Forward declarations

namespace dungeon
{
    class Data;
}

namespace states
{
    //! Where to unlock monsters in the hub.

    class HubInn final : public State
    {
        using baseClass = State;

    public:

        //! Constructor.
        HubInn(StateStack& stack, dungeon::Data& data);

        //! Default destructor.
        ~HubInn() = default;

        //----------------------//
        //! @name State control
        //! @{

        StateID id() const noexcept final { return StateID::HUB_INN; }

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
        std::vector<hub::MonsterLockerPtr> m_monsterLockers;    //!< Monsters list.

        // Decorum
        scene::Label m_title;                   //!< Title.
        scene::RectangleGradient m_background;  //!< Background.
    };
}
