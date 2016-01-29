#pragma once

#include "states/state.hpp"
#include "states/identifiers.hpp"
#include "scene/wrappers/animatedsprite.hpp"
#include "dcb/gaugesmanager.hpp"
#include "dcb/answerbox.hpp"
#include "dcb/bubble.hpp"
#include "dcb/controller.hpp"
#include "nui/pushbutton.hpp"
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

        //! Destructor.
        ~GameDCB();

        StateID id() const noexcept final { return StateID::GAME_DCB; }

    protected:

        //---------------//
        //! @name Events
        //! @{

        void handleEvent(const sf::Event& event) final;

        //! @}

        //--------------------//
        //! @name Interaction
        //! @{

        //! Check dungeon creation validity and shows a conclusion message.
        void beforeConfirmDungeonCreation();

        //! Creates the dungeon given the current data.
        void confirmDungeonCreation();

        //! Called when name is validated.
        void onNameValidate();

        //! @}

    private:

        nui::HStacker m_nameStacker;    //!< Stacks name asking entities.
        nui::TextEntry m_nameEntry;     //!< The entry for the dungeon name.
        nui::PushButton m_okButton;     //!< The only one button in the world.

        dcb::Bubble m_bubble;               //!< The message.
        dcb::AnswerBox m_answerBox;         //!< The box with all possible answers.
        dcb::GaugesManager m_gaugesManager; //!< The gauges.
        dcb::Controller m_controller;       //!< Controls the content of the other DCB elements.

        // Confirm
        bool m_confirmGoing = false;    //!< True if the negociation phase has ended.
        bool m_confirmStatus = false;   //!< If going, is there enough conviction to accept dungeon?

        // Decorum
        scene::AnimatedSprite m_trees;        //!< The animated trees.
        scene::AnimatedSprite m_stage;        //!< The animated stage.
        scene::AnimatedSprite m_candle;       //!< The animated candle.
    };
}
