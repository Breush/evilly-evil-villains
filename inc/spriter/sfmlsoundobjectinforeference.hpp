#ifndef SFMLSOUNDOBJECTINFOREFERENCE_H
#define SFMLSOUNDOBJECTINFOREFERENCE_H

#include <Spriter/override/soundobjectinforeference.h>

#include <SFML/Audio/Sound.hpp>

namespace SpriterEngine
{


    class SfmlSoundObjectInfoReference : public SoundObjectInfoReference
    {
    public:
        SfmlSoundObjectInfoReference(sf::SoundBuffer &buffer);

        void playTrigger() override;

    private:
        void setPlaybackVolume() override;
        void setPlaybackPanning() override;

        sf::Sound sound;
    };

}

#endif // SFMLSOUNDOBJECTINFOREFERENCE_H
