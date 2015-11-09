#include "spriter/sfmlsoundobjectinforeference.hpp"

#include "core/application.hpp"

#include <iostream>

namespace SpriterEngine
{
    // TODO Use Application holders

    SfmlSoundObjectInfoReference::SfmlSoundObjectInfoReference(std::string soundID)
        : m_soundID(soundID)
    {
    }

    void SfmlSoundObjectInfoReference::playTrigger()
    {
        if (getTriggerCount())
        {
            // FIXME Too much sounds!
            // std::cerr << "Play " << m_soundID << std::endl;
            Application::context().sounds.play(m_soundID);

            // TODO Use volume
        }
    }

    void SfmlSoundObjectInfoReference::setPlaybackVolume()
    {
    }
}
