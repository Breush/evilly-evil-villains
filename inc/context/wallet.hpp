#pragma once

#include "tools/int.hpp"

#include <string>

// Forward declarations

namespace context
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
        bool add(uint addValue);

        //! Tries to sub some value from the current one in the wallet.
        //! Does nothing if impossible and returns false.
        bool sub(uint subValue);

        //! Tries to add and sub some value to the wallet.
        //! Does nothing if impossible and returns false.
        //! Prevents overflow and returns false if occured.
        bool addsub(uint addValue, uint subValue);

        //! @}

        //----------------//
        //! @name Control
        //!

        //! Sets what to use for events.
        void setEvents(EventEmitter* emitter, std::string eventType);

        //! Set a factor applied to each modification (except set).
        void setFactor(uint factor);

        //! @}

    private:

        uint m_value = 0u;  //!< The value, instead.
        uint m_factor = 1u; //!< Multiplicative factor applied to each modification.

        EventEmitter* m_emitter = nullptr; //!< The emitter to use on changes.
        std::string m_eventType;           //!< The type of events to use on changes.
    };
}
