#include "spriter/spriterfilefactory.hpp"

#include <Spriter/override/imagefile.h>
#include <Spriter/override/soundfile.h>

#include "spriter/pugixmlspriterfiledocumentwrapper.hpp"
#include "spriter/sfmlimagefile.hpp"
#include "spriter/sfmlsoundfile.hpp"

using namespace SpriterEngine;

SpriterFileFactory::SpriterFileFactory()
{
}

//--------------------//
//----- Wrappers -----//

ImageFile* SpriterFileFactory::newImageFile(const std::string &initialFilePath, point initialDefaultPivot)
{
    return new SfmlImageFile(initialFilePath, initialDefaultPivot);
}

SoundFile* SpriterFileFactory::newSoundFile(const std::string & initialFilePath)
{
    return new SfmlSoundFile(initialFilePath);
}

SpriterFileDocumentWrapper* SpriterFileFactory::newScmlDocumentWrapper()
{
    return new PugiXmlSpriterFileDocumentWrapper();
}
