#pragma once

#include "tools/debug.hpp"

#include <vector>

// TODO Move that file to context.

namespace dungeon
{
    // Forward declarations

    class EventReceiver;
    enum class EventType;
    struct Event;

    //! Base class for those who want to be able to emit a dungeon event.

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
        void emit(const Event& event) const;

        //! Emits a default event type through all receivers.
        virtual void emit(EventType eventType) = 0;

        //! @}

    private:

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
    };

    //! Base class for those who want to be able to receive a dungeon event.

    class EventReceiver
    {
        friend class EventEmitter;

    public:

        //! Destructor.
        virtual ~EventReceiver();

        //---------------//
        //! @name Events
        //! @{

        //! Set the event emitter.
        void setEmitter(EventEmitter* emitter);

        //! Get the event emitter.
        inline EventEmitter* emitter() { return m_emitter; };

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
        //! In debug mode, be sure that the receiver is not detroyed during emit.
        bool m_lock = false;
#endif
    };
}
