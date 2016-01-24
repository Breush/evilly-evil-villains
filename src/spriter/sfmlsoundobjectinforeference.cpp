#include "spriter/sfmlsoundobjectinforeference.hpp"

#include "context/context.hpp"

using namespace SpriterEngine;

SfmlSoundObjectInfoReference::SfmlSoundObjectInfoReference(std::string soundID)
    : m_soundID(soundID)
{
}

void SfmlSoundObjectInfoReference::playTrigger()
{
    if (getTriggerCount() > 0) {
        context::context.sounds.play(m_soundID);
    }
}
