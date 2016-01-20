#pragma once

#include "tools/platform-fixes.hpp"
#include "tools/debug.hpp"
#include "tools/int.hpp"

#include <string>
#include <vector>
#include <deque>

namespace context
{
    // Forward declarations

    class EventReceiver;

    //! Base class for event.

    struct Event
    {
        virtual ~Event() = default;

        std::string type;   //!< The type of event which is sent.
    };

    //! Base class to be able to emit an event.

    class EventEmitter
    {
        friend class EventReceiver;

    public:

        //! Default destructor.
        virtual ~EventEmitter() = default;

        //----------------------//
        //! @name Event emitter
        //! @{

        //! Emits an event through all receivers.
        //! Note: similar events won't be launched twice.
        //! Set direct to true to send the event without adding it to the broadcast list.
        void addEvent(std::unique_ptr<Event> event, bool direct = false);

        //! Emits a default event type through all receivers.
        //! Note: similar events won't be launched twice.
        //! Set direct to true to send the event without adding it to the broadcast list.
        void addEvent(std::string eventType, bool direct = false);

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        //! Emits stored events through all receivers (or count if specified).
        void broadcast(uint count = -1u);

        //! @}

        //-------------------------//
        //! @name Manage receivers
        //! @{

        //! Method called by a receiver to register itself.
        void addReceiver(EventReceiver* receiver);

        //! Method called by a receiver to unregister itself.
        void removeReceiver(EventReceiver* receiver);

        //! @}

    private:

        //! All the receivers to be called when an event is sent.
        std::vector<EventReceiver*> m_receivers;

        //! The list of events to broadcasts.
        std::deque<std::unique_ptr<Event>> m_events;
    };

    //! Base class for those who want to be able to receive a dungeon event.

    class EventReceiver
    {
        friend class EventEmitter;

    public:

        //! Default constructor.
        EventReceiver() = default;

        //! Destructor.
        virtual ~EventReceiver();

        //---------------//
        //! @name Events
        //! @{

        //! Set the event emitter.
        void setEmitter(EventEmitter* emitter);

        //! Get the event emitter.
        inline EventEmitter* emitter() { return m_emitter; }

        //! @}

    protected:

        //---------------//
        //! @name Events
        //! @{

        //! Called whenever an event is sent.
        virtual void receive(const Event& event) = 0;

        //! @}

    private:

        //! The emitter events are coming from.
        EventEmitter* m_emitter = nullptr;

#if DEBUG_GLOBAL > 0
        //! In debug mode, assert that the receiver is not detroyed during emit.
        bool m_lock = false;
#endif
    };
}
