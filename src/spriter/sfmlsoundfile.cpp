#include "spriter/sfmlsoundfile.hpp"

#include "core/application.hpp"
#include "spriter/sfmlsoundobjectinforeference.hpp"

#include <iostream>

using namespace SpriterEngine;

SfmlSoundFile::SfmlSoundFile(std::string initialFilePath) :
    SoundFile(initialFilePath)
{
    m_soundID = Application::context().sounds.getID(path());
}

SoundObjectInfoReference* SfmlSoundFile::newSoundInfoReference()
{
    // Note: Should be deleted by whoever calls it
    return new SfmlSoundObjectInfoReference(m_soundID);
}
