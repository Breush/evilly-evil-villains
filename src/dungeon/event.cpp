#include "dungeon/event.hpp"

#include <algorithm>

using namespace dungeon;

//--------------------------//
//----- Event receiver -----//

void EventReceiver::setEmitter(EventEmitter* emitter)
{
    if (m_emitter != nullptr)
        m_emitter->removeReceiver(this);

    m_emitter = emitter;

    if (m_emitter != nullptr)
        m_emitter->addReceiver(this);
}

EventReceiver::~EventReceiver()
{
    setEmitter(nullptr);
}

//-------------------------//
//----- Event emitter -----//

void EventEmitter::emit(const Event& event)
{
    for (auto& receiver : m_receivers)
        receiver->receive(event);
}

//-------------------------------//
//----- Receiver management -----//

void EventEmitter::addReceiver(EventReceiver* receiver)
{
    m_receivers.push_back(receiver);
}

void EventEmitter::removeReceiver(EventReceiver* receiver)
{
    std::remove_if(m_receivers.begin(), m_receivers.end(),
        [=](EventReceiver* inReceiver) { return receiver == inReceiver; });
}
