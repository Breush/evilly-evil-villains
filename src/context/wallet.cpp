#include "context/wallet.hpp"

#include "dungeon/event.hpp"
#include "tools/tools.hpp"

using namespace context;

//-----------------------//
//----- Interaction -----//

void Wallet::set(const uint value)
{
    if (m_value != value) {
        m_value = value;

        // Send event if we should
        if (m_emitter != nullptr)
            m_emitter->emit(m_eventType);
    }
}

bool Wallet::add(const uint addValue)
{
    // Overflow
    if (m_value + addValue < m_value) {
        set(-1u);
        return false;
    }

    // Normal add
    set(m_value + addValue);
    return true;
}

bool Wallet::sub(const uint subValue)
{
    // Impossible to substract
    returnif (m_value < subValue) false;

    // Normal sub
    set(m_value - subValue);
    return true;
}

bool Wallet::addsub(const uint addValue, const uint subValue)
{
    // Impossible to substract
    returnif (m_value + addValue < subValue) false;

    // Overflow
    if (m_value + addValue < m_value) {
        set(-1u - subValue);
        return false;
    }

    // Normal addsub
    set(m_value + addValue - subValue);
    return true;
}

//-----------------------------//
//----- Events management -----//

void Wallet::setEvents(dungeon::EventEmitter* emitter, dungeon::EventType eventType)
{
    m_emitter = emitter;
    m_eventType = eventType;
}
