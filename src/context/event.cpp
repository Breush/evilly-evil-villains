#include "context/event.hpp"

#include "tools/platform-fixes.hpp" // erase_if

#include <stdexcept>

using namespace context;

//--------------------------//
//----- Event receiver -----//

EventReceiver::~EventReceiver()
{
#if DEBUG_GLOBAL > 0
    if (m_lock)
        throw std::logic_error("Receiver is locked, you can not destroy it, please delay that somehow.");
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
}

//-------------------------//
//----- Event emitter -----//

void EventEmitter::emit(const Event& event) const
{
    // Make a copy of the current list
    // so that it can be modify during the emit
    auto receivers = m_receivers;

#if DEBUG_GLOBAL > 0
    for (auto& receiver : receivers)
        receiver->m_lock = true;
#endif

    for (auto& receiver : receivers)
        receiver->receive(event);

#if DEBUG_GLOBAL > 0
    for (auto& receiver : receivers)
        receiver->m_lock = false;
#endif
}

void EventEmitter::emit(std::string eventType) const
{
    Event event{std::move(eventType)};
    emit(event);
}

//-------------------------------//
//----- Receiver management -----//

void EventEmitter::addReceiver(EventReceiver* receiver)
{
    m_receivers.push_back(receiver);
}

void EventEmitter::removeReceiver(EventReceiver* receiver)
{
    std::erase_if(m_receivers, [=](EventReceiver* inReceiver) { return receiver == inReceiver; });
}
