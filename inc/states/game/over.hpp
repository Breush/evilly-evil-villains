#pragma once

#include "states/state.hpp"
#include "states/identifiers.hpp"
#include "scene/wrappers/label.hpp"

namespace states
{
    class GameOver final : public State
    {
        using baseClass = State;

    public:

        //! Constructor.
        GameOver(StateStack& stack);

        //! Default destructor.
        ~GameOver();

        StateID id() const noexcept final { return StateID::GAME_OVER; }

    protected:

        //----------------//
        //! @name Routine
        //! @{

        bool update(const sf::Time& dt) final;
        void handleEvent(const sf::Event& event) final;
        void refreshWindow(const config::WindowInfo& cWindow) final;

        //! @}

    private:

        // Decorum
        scene::Label m_gameOver;    //!< Game over string.
    };
}
