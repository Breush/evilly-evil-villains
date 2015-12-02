#include "spriter/sfmlsoundobjectinforeference.hpp"

#include "core/application.hpp"

using namespace SpriterEngine;

SfmlSoundObjectInfoReference::SfmlSoundObjectInfoReference(std::string soundID)
    : m_soundID(soundID)
{
}

void SfmlSoundObjectInfoReference::playTrigger()
{
    if (getTriggerCount() > 0)
    {
        Application::context().sounds.play(m_soundID);

        // TODO Use volume
    }
}
