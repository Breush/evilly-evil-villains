#pragma once

#include "scene/entity.hpp"
#include "context/event.hpp"
#include "context/logger.hpp"
#include "sfe/twraptext.hpp"
#include "nui/textentry.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

#include <deque>

namespace dungeon
{

    //! A logging window for dungeons events.
    //! The events are faded to only keep recent ones.

    class Log final : public scene::Entity, public context::EventReceiver, public context::Logger
    {
        using baseClass = scene::Entity;

    public:

        //! Decides the direction of stacking messages.
        enum class Direction
        {
            OLD_BELOW,
            NEW_BELOW,
        };

    public:

        //! Constructor.
        Log();

        //! Default destructor.
        ~Log() = default;

        //----------------//
        //! @name Routine
        //! @{

        //! Add a message to the log.
        void addMessage(std::wstring text, sf::Color color = sf::Color::White);

        //! @}

        //--------------------------//
        //! @name Public properties
        //! @{

        //! Size override, as height is auto-controled.
        PARAMGSU(float, m_width, width, setWidth, refreshSize)

        //! Decides the direction of stacking messages.
        PARAMGSU(Direction, m_direction, direction, setDirection, refreshMessages)

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onSizeChanges() final;
        void updateRoutine(const sf::Time& dt) final;
        void refreshNUI(const config::NUIGuides& cNUI) final;

        //! @}

        //---------------//
        //! @name Events
        //! @{

        void receive(const context::Event& event) final;

        //! @}

        //-----------------//
        //! @name Commands
        //! @{

        void commandLog(const std::wstring& message) final;

        //! Callback when command entry is validated.
        void onCommandEntryValidated();

        //! @}

        //--------------------------------//
        //! @name Internal change updates
        //! @{

        //! Recomputes the position of all messages.
        void refreshMessages();

        //! Refresh the fit width for all messages.
        void refreshTextsWrap();

        //! Called to recompute the size from width.
        void refreshSize();

        //! Called to recompute the size.
        void refreshHeight();

        //! @}

        //! A displayed message.
        struct Message
        {
            sfe::WrapText<sf::Text> text;   //!< The visible part of the message, with return to line if needed.
            float aliveSince = 0.f;         //!< The time elapsed since the message was created, in seconds.
        };

    private:

        // Messages
        std::deque<Message> m_messages; //!< The currently alive messages.
        float m_aliveTime = 5.f;        //!< How long to keep a message alive, in seconds.
        float m_currentHeight = 0.f;    //!< The current height of the displayed messages.

        // Command
        nui::TextEntry m_commandEntry;  //!< The command entry.

        // Decorum
        sf::RectangleShape m_background;    //!< The soft background, fitting the current size.

        // TODO Make a flyweight with all these
        float m_characterSize = 0.f;        //!< The font size.
    };
}
