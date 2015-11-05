#include "spriter/sfmlsoundobjectinforeference.hpp"

#include <iostream>

namespace SpriterEngine
{

    SfmlSoundObjectInfoReference::SfmlSoundObjectInfoReference(sf::SoundBuffer &buffer)
    {
        sound.setBuffer(buffer);
    }

    void SfmlSoundObjectInfoReference::playTrigger()
    {
        if (getTriggerCount())
        {
            sound.play();
        }
    }

    void SfmlSoundObjectInfoReference::setPlaybackVolume()
    {
        std::cerr << "V " << getVolume() << std::endl;
        sound.setVolume(100 * getVolume());
    }

    void SfmlSoundObjectInfoReference::setPlaybackPanning()
    {
        std::cerr << "P " << getPanning() << std::endl;
        sound.setPosition(100 * getPanning(), 0, 0);
    }

}
