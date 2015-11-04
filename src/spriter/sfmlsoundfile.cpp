#include "spriter/sfmlsoundfile.hpp"

#include "spriter/sfmlsoundobjectinforeference.hpp"

namespace SpriterEngine
{
    SfmlSoundFile::SfmlSoundFile(std::string initialFilePath) :
        SoundFile(initialFilePath)
    {
        initializeFile();
    }

    void SfmlSoundFile::initializeFile()
    {
        if (!buffer.loadFromFile(path()))
        {
            // error
        }
    }

    SoundObjectInfoReference * SfmlSoundFile::newSoundInfoReference()
    {
        return new SfmlSoundObjectInfoReference(buffer);
    }

}
