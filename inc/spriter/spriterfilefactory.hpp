#pragma once

#include <Spriter/override/filefactory.h>

namespace SpriterEngine
{
    //! File factory based on SpriterPlusPlus interface.

    class SpriterFileFactory final : public FileFactory
    {
    public:

        //! Constructor.
        SpriterFileFactory();

        //-----------------//
        //! @name Wrappers
        //! @{

        ImageFile* newImageFile(const std::string &initialFilePath, point initialDefaultPivot) final;
        SoundFile* newSoundFile(const std::string &initialFilePath) final;
        SpriterFileDocumentWrapper* newScmlDocumentWrapper() final;

        //! @}
    };
}
