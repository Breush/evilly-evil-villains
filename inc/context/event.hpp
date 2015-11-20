#pragma once

#include "tools/debug.hpp"

#include <string>
#include <vector>

namespace context
{
    // Forward declarations

    class EventReceiver;

    //! Base class for event.

    struct Event
    {
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
        void emit(const Event& event) const;

        //! Emits a default event type through all receivers.
        void emit(std::string eventType) const;

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
        //! In debug mode, assert that the receiver is not detroyed during emit.
        bool m_lock = false;
#endif
    };
}
