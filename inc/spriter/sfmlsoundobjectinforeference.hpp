#pragma once

#include <Spriter/override/soundobjectinforeference.h>

#include <SFML/Audio/Sound.hpp>

namespace SpriterEngine
{
    class SfmlSoundObjectInfoReference final : public SoundObjectInfoReference
    {
    public:

        SfmlSoundObjectInfoReference(std::string soundID);

        void playTrigger() final;

    private:

        void setPlaybackVolume() final;

    private:

        std::string m_soundID;
    };
}
