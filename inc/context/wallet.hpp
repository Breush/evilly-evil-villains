#pragma once

#include "dungeon/eventtype.hpp"

// Forward declarations

namespace dungeon
{
    class EventEmitter;
}

namespace context
{
    //! Stores and controls some value.
    //! For example, used as dosh manager for a villain.

    class Wallet final
    {
    public:

        //! Default constructor.
        Wallet() = default;

        //! Default destructor.
        ~Wallet() = default;

        //--------------------//
        //! @name Interaction
        //! @{

        //! The stored value.
        inline uint value() const { return m_value; }

        //! Set the value to a specific one.
        void set(const uint value);

        //! Add some value to the wallet.
        //! Prevents overflow and returns false if occured.
        bool add(const uint addValue);

        //! Tries to sub some value from the current one in the wallet.
        //! Does nothing if impossible and returns false.
        bool sub(const uint subValue);

        //! Tries to add and sub some value to the wallet.
        //! Does nothing if impossible and returns false.
        //! Prevents overflow and returns false if occured.
        bool addsub(const uint addValue, const uint subValue);

        //! @}

        //--------------------------//
        //! @name Events management
        //!

        //! Sets what to use for events.
        void setEvents(dungeon::EventEmitter* emitter, dungeon::EventType eventType);

        //! @}

    private:

        uint m_value = 0u;  //!< The value, instead.

        dungeon::EventEmitter* m_emitter = nullptr; //!< The emitter to use on changes.
        dungeon::EventType m_eventType;             //!< The type of events to use on changes.
    };
}
