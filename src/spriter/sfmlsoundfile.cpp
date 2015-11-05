#include "spriter/sfmlsoundfile.hpp"

#include "spriter/sfmlsoundobjectinforeference.hpp"

#include <iostream>

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
            std::cerr << "Error: cannot load " << path() << std::endl;
            // error
        }
    }

    SoundObjectInfoReference * SfmlSoundFile::newSoundInfoReference()
    {
        return new SfmlSoundObjectInfoReference(buffer);
    }

}
