#include "dungeon/event.hpp"

#include "tools/platform-fixes.hpp" // erase_if

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

void EventEmitter::emit(const Event& event) const
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
    std::erase_if(m_receivers, [=](EventReceiver* inReceiver) { return receiver == inReceiver; });
}
