#include "override/filefactory.h"

#include "override/imagefile.h"
#include "override/soundfile.h"

#include "override/spriterfiledocumentwrapper.h"


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
        return 0;
    }

    SpriterFileDocumentWrapper * FileFactory::newSconDocumentWrapper()
    {
        return 0;
    }

}
