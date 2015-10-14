#pragma once

#include "context/logger.hpp"
#include "states/state.hpp"
#include "states/identifiers.hpp"
#include "scene/wrappers/rectangleshape.hpp"
#include "scene/wrappers/wraplabel.hpp"
#include "nui/textentry.hpp"

#include <memory>

namespace states
{
    //! State shown when the user wants to quit the game.

    class Terminal final : public State, public context::Logger
    {
        using baseClass = State;

    public:

        //! Constructor.
        Terminal(StateStack& stack);

        //! Default destructor.
        ~Terminal() = default;

        //----------------------//
        //! @name State control
        //! @{

        StateID id() const noexcept final { return StateID::TERMINAL; }

        //! @}

        //-----------------//
        //! @name Messages
        //! @{

        //! Add a message to the log.
        void addMessage(std::wstring text, sf::Color color = sf::Color::White);

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        bool update(const sf::Time& dt) final;
        void handleEvent(const sf::Event& event) final;
        void refreshNUI(const config::NUIGuides& cNUI) final;
        void refreshWindow(const config::WindowInfo& cWindow) final;

        //! @}

        //-----------------//
        //! @name Commands
        //! @{

        void commandLog(const std::wstring& message) final;

        //! Callback when command entry is validated.
        void onEntryValidated();

        //! @}

        //--------------------------------//
        //! @name Internal change updates
        //! @{

        //! Recomputes the position of all messages.
        void refreshMessagesPositions();

        //! Refresh the fit width for all messages.
        void refreshFromWidth();

        //! Refresh the size from width and height.
        void refreshSize();

        //! @}

        //! A displayed message.
        struct Message
        {
            std::unique_ptr<scene::WrapLabel<sf::Text>> label;  //!< The visible part of the message, with return to line if needed.
            float aliveSince = 0.f;                             //!< The time elapsed since the message was created, in seconds.
        };

    private:

        // Messages
        std::deque<Message> m_messages; //!< The currently alive messages.
        float m_aliveTime = 7.f;        //!< How long to keep a message alive, in seconds.

        // Decorum
        scene::RectangleShape m_background; //!< Background.
        nui::TextEntry m_entry;             //!< The command entry.
        float m_width = 0.f;                //!< The width.
        float m_currentHeight = 0.f;        //!< The height.
        float m_characterSize = 0.f;        //!< The font size.
    };
}
