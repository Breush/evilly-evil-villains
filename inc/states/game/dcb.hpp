#pragma once

#include "states/state.hpp"
#include "states/identifiers.hpp"
#include "sfe/animatedsprite.hpp"
#include "dcb/gaugesmanager.hpp"
#include "dcb/answerbox.hpp"
#include "dcb/bubble.hpp"
#include "dcb/controller.hpp"
#include "nui/button.hpp"
#include "nui/textentry.hpp"

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
        void confirmDungeonCreation();

        //! Called when name is validated.
        void onNameValidate();

    private:

        sfe::AnimatedSprite m_background;   //!< The animated background.
        nui::TextEntry m_nameEntry;         //!< The entry for the dungeon name.
        nui::Button m_okButton;             //!< The only one button in the world.

        dcb::Bubble m_bubble;               //!< The message.
        dcb::AnswerBox m_answerBox;         //!< The box with all possible answers.
        dcb::GaugesManager m_gaugesManager; //!< The gauges.
        dcb::Controller m_controller;       //!< Controls the content of the other DCB elements.
    };
}
