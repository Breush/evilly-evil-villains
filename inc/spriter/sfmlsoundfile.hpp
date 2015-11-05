#pragma once

#include <Spriter/override/soundfile.h>

#include <SFML/Audio/SoundBuffer.hpp>

namespace SpriterEngine
{

    class SfmlSoundFile : public SoundFile
    {
    public:
        SfmlSoundFile(std::string initialFilePath);

        SoundObjectInfoReference * newSoundInfoReference();

    protected:

        void initializeFile();

    private:

        std::string m_soundID;
    };

}

