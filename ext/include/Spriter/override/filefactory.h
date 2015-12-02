#ifndef FILEFACTORY_H
#define FILEFACTORY_H

#include <string>

#include "../global/global.h"

namespace SpriterEngine
{

    class ImageFile;
    class SoundFile;
    class SpriterFileDocumentWrapper;

    class FileFactory
    {
    public:
        FileFactory();
        virtual ~FileFactory();

        virtual ImageFile *newImageFile(const std::string &initialFilePath, point initialDefaultPivot) = 0;
        virtual SoundFile *newSoundFile(const std::string &initialFilePath) = 0;

        virtual SpriterFileDocumentWrapper *newScmlDocumentWrapper() { return nullptr; }
        virtual SpriterFileDocumentWrapper *newSconDocumentWrapper() { return nullptr; }
    };

}

#endif // FILEFACTORY_H
