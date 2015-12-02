#pragma once

#include <Spriter/override/soundfile.h>

#include <SFML/Audio/SoundBuffer.hpp>

namespace SpriterEngine
{

    class SfmlSoundFile final : public SoundFile
    {
    public:

        SfmlSoundFile(std::string initialFilePath);

        SoundObjectInfoReference* newSoundInfoReference() final;

    private:

        std::string m_soundID;
    };

}

