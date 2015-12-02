#pragma once

#include "states/state.hpp"
#include "states/identifiers.hpp"
#include "nui/hstacker.hpp"
#include "nui/button.hpp"
#include "scene/wrappers/rectanglegradient.hpp"
#include "scene/wrappers/label.hpp"

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

    private:

        // NUI
        nui::Button m_button;       //!< Back button.

        // Decorum
        scene::Label m_title;                   //!< Title.
        scene::RectangleGradient m_background;  //!< Background.
    };
}
