#include "context/event.hpp"

#include "tools/tools.hpp"
#include "tools/math.hpp"

#include <stdexcept>

using namespace context;

//--------------------------//
//----- Event receiver -----//

EventReceiver::~EventReceiver()
{
#if DEBUG_GLOBAL > 0
    if (m_lock) {
        std::cerr << "ERROR: Receiver is locked, you can not destroy it, please delay that somehow." << std::endl;
        std::cerr << "       If an exception follows, this message might not be relevant." << std::endl;
    }
#endif

    setEmitter(nullptr);
}

void EventReceiver::setEmitter(EventEmitter* emitter)
{
    if (m_emitter != nullptr)
        m_emitter->removeReceiver(this);

    m_emitter = emitter;

    if (m_emitter != nullptr)
        m_emitter->addReceiver(this);

#if DEBUG_GLOBAL > 0
    if (m_emitter == nullptr)
        m_lock = false;
#endif
}

//-------------------------//
//----- Event emitter -----//

void EventEmitter::broadcast(uint count)
{
    count = std::min(count, static_cast<uint>(m_events.size()));
    returnif (count == 0u);

#if DEBUG_GLOBAL > 0
    for (auto& receiver : m_receivers)
        receiver->m_lock = true;
#endif

    for (uint i = 0u; i < count; ++i) {
        const auto& event = m_events[i];
        for (auto& receiver : m_receivers)
            receiver->receive(*event);
    }

    m_events.erase(std::begin(m_events), std::begin(m_events) + count);

#if DEBUG_GLOBAL > 0
    for (auto& receiver : m_receivers)
        receiver->m_lock = false;
#endif
}

void EventEmitter::addEvent(std::unique_ptr<Event> event, bool direct)
{
    // Send it
    if (direct) {
#if DEBUG_GLOBAL > 0
        for (auto& receiver : m_receivers)
            receiver->m_lock = true;
#endif

        for (auto& receiver : m_receivers)
            receiver->receive(*event);

#if DEBUG_GLOBAL > 0
        for (auto& receiver : m_receivers)
            receiver->m_lock = false;
#endif
    }

    // Add it to the broadcast list
    else {
        // TODO Do not add it twice if event already exists in the list
        // Be careful, though, because something like
        //  - facility_changed 0 1
        //  - room_destroyed 0 1
        //  - room_created 0 1
        //  - facility_changed 0 1
        // Could drop the very useful last facility_changed
        // Two possibilities :
        //  - Drop the event if the last is the same (quick check and no problem with that)
        //  - OR Move the event to the end. Leaving it with one copy but being sure the other things are done well before
        //      (checking the list from the end would be the quickest)
        m_events.emplace_back(std::move(event));
    }
}

void EventEmitter::addEvent(std::string eventType, bool direct)
{
    auto event = std::make_unique<Event>();
    event->type = std::move(eventType);
    addEvent(std::move(event), direct);
}

//-------------------------------//
//----- Receiver management -----//

void EventEmitter::addReceiver(EventReceiver* receiver)
{
    m_receivers.emplace_back(receiver);
}

void EventEmitter::removeReceiver(EventReceiver* receiver)
{
    std::erase_if(m_receivers, [=](EventReceiver* inReceiver) { return receiver == inReceiver; });
}
