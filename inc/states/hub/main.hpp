#pragma once

#include "states/state.hpp"
#include "states/identifiers.hpp"
#include "hub/selector.hpp"

// Forward declarations

namespace dungeon
{
    class Data;
}

namespace states
{
    //! Central state for game.

    class HubMain final : public State
    {
        using baseClass = State;

    public:

        //! Constructor...
        HubMain(StateStack& stack, dungeon::Data& data);

        //! Destructor.
        ~HubMain();

        StateID id() const noexcept final { return StateID::HUB_MAIN; }

    protected:

        //----------------//
        //! @name Routine
        //! @{

        bool update(const sf::Time& dt) final;
        void handleEvent(const sf::Event& event) final;

        //! @}

    private:

        // Decorum
        hub::Selector m_selector;   //!< Houses (market/inn/etc.) selector.
    };
}
