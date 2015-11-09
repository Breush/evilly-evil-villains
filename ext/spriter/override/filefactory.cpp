#include "override/filefactory.h"

#include "override/imagefile.h"
#include "override/soundfile.h"

namespace SpriterEngine
{

    FileFactory::FileFactory()
    {
    }

    FileFactory::~FileFactory()
    {
    }

    ImageFile * FileFactory::newImageFile(const std::string &initialFilePath, point initialDefaultPivot)
    {
        return new ImageFile(initialFilePath, initialDefaultPivot);
    }

    SoundFile * FileFactory::newSoundFile(const std::string &initialFilePath)
    {
        return new SoundFile(initialFilePath);
    }

    SpriterFileDocumentWrapper * FileFactory::newScmlDocumentWrapper()
    {
        return nullptr;
    }

    SpriterFileDocumentWrapper * FileFactory::newSconDocumentWrapper()
    {
        return nullptr;
    }

}
