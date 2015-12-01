#pragma once

#include "states/state.hpp"
#include "states/identifiers.hpp"
#include "hub/selector.hpp"

namespace states
{
    //! Central state for game.

    class Hub : public State
    {
        using baseClass = State;

    public:

        //! Constructor.
        Hub(StateStack& stack);

        //! Destructor.
        ~Hub();

        StateID id() const noexcept final { return StateID::HUB; }

    protected:

        //----------------//
        //! @name Routine
        //! @{

        bool update(const sf::Time& dt) final;
        void handleEvent(const sf::Event& event) final;

        //! @}

    private:

        hub::Selector m_selector;
    };
}
