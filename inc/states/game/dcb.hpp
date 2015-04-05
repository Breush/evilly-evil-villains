#pragma once

#include "states/state.hpp"
#include "states/identifiers.hpp"
#include "sfe/rectangleshape.hpp"
#include "nui/button.hpp"

namespace states
{
    //! State of in-game Dungeon Community Bank.

    class GameDCB : public State
    {
        using baseClass = State;

    public:

        //! Constructor.
        GameDCB(StateStack& stack);

        //! Default destructor.
        ~GameDCB() final = default;

        //! ID of the state.
        StateID id() const noexcept final { return StateID::GAME_DCB; }

    protected:

        //! Creates the dungeon given the current data.
        void createDungeon();

    private:

        sfe::RectangleShape m_background;   //!< The background image.
        nui::Button m_button;               //!< The only one button in the world.
    };
}
