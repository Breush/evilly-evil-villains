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
            //sound.play();

            // TODO Use volume
        }
    }

    void SfmlSoundObjectInfoReference::setPlaybackVolume()
    {
    }
}
