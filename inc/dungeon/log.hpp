#pragma once

#include "scene/entity.hpp"
#include "dungeon/event.hpp"
#include "sfe/wraptext.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

#include <deque>

namespace dungeon
{

    //! A logging window for dungeons events.
    //! The events are faded to only keep recent ones.

    class Log final : public scene::Entity, public EventReceiver
    {
        using baseClass = scene::Entity;

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

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onSizeChanges() final;
        void updateRoutine(const sf::Time& dt) final;
        void refreshDisplay() final;

        //! @}

        //---------------//
        //! @name Events
        //! @{

        void receive(const Event& event) final;

        //! @}

        //--------------------------------//
        //! @name Internal change updates
        //! @{

        //! Recomputes the position of all messages.
        void refreshMessages();

        //! Refresh the fit width for all messages.
        void refreshTextsWrap();

        //! Refresh the size of the background to fit the height of the messages.
        void refreshBackgroundSize();

        //! @}

        //! A displayed message.
        struct Message
        {
            sfe::WrapText text;     //!< The visible part of the message, with return to line if needed.
            float aliveSince = 0.f; //!< The time elapsed since the message was created, in seconds.
        };

    private:

        std::deque<Message> m_messages; //!< The currently alive messages.

        float m_aliveTime = 5.f;        //!< How long to keep a message alive, in seconds.
        float m_currentHeight = 0.f;    //!< The current height of the displayed messages.

        sf::RectangleShape m_background;    //!< The soft background, fitting the current size.
        
        float m_characterSize = 0.f;    //!< The font size.
    };
}
