#pragma once

#include "states/state.hpp"
#include "states/identifiers.hpp"
#include "nui/scrollarea.hpp"
#include "nui/hstacker.hpp"
#include "nui/vstacker.hpp"
#include "nui/button.hpp"
#include "scene/wrappers/rectanglegradient.hpp"
#include "scene/wrappers/richlabel.hpp"
#include "scene/wrappers/label.hpp"

#include <array>

namespace states
{
    class HubMarket : public State
    {
        using baseClass = State;

    public:

        //! Constructor.
        HubMarket(StateStack& stack);

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

        // NUI
        nui::ScrollArea m_scrollArea;               //!< The scroll area.
        nui::HStacker m_globalStacker;              //!< Stacker inside the scrollarea.
        std::array<nui::VStacker, 2u> m_columns;    //!< The stackers forming the columns.
        nui::Button m_button;                       //!< Back button.

        // Lockers
        using TrapLocker = scene::RichLabel;
        std::vector<std::unique_ptr<TrapLocker>> m_trapLockers; //!< Available traps.

        // Decorum
        scene::Label m_title;                   //!< Title.
        scene::RectangleGradient m_background;  //!< Background.
    };
}
