#include "override/imagefile.h"

#include "objectinfo/universalobjectinterface.h"

namespace SpriterEngine
{

    ImageFile::ImageFile(std::string initialFilePath, point initialDefaultPivot) :
        File(initialFilePath),
        defaultPivot(initialDefaultPivot)
    {
    }

    ImageFile *ImageFile::imageFile()
    {
        return this;
    }
}
