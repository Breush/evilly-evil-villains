#include "context/wallet.hpp"

#include "context/event.hpp"
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
            m_emitter->addEvent(m_eventType);
    }
}

bool Wallet::required(uint subValue)
{
    subValue *= m_factor;
    return m_value >= subValue;
}

bool Wallet::add(uint addValue)
{
    addValue *= m_factor;

    // Overflow
    if (m_value + addValue < m_value) {
        set(-1u);
        return false;
    }

    // Normal add
    set(m_value + addValue);
    return true;
}

bool Wallet::sub(uint subValue)
{
    subValue *= m_factor;

    // Impossible to substract
    returnif (m_value < subValue) false;

    // Normal sub
    set(m_value - subValue);
    return true;
}

bool Wallet::addsub(uint addValue, uint subValue)
{
    addValue *= m_factor;
    subValue *= m_factor;

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

//-------------------//
//----- Control -----//

void Wallet::setEvents(context::EventEmitter* emitter, std::string eventType)
{
    m_emitter = emitter;
    m_eventType = std::move(eventType);
}

void Wallet::setFactor(uint factor)
{
    m_factor = factor;
}
