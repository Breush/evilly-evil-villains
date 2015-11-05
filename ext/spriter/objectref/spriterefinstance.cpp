#include "objectref/spriterefinstance.h"

#include "objectinfo/universalobjectinterface.h"
#include "file/filereference.h"
#include "override/imagefile.h"

#include <iostream>

namespace SpriterEngine
{

    SpriteRefInstance::SpriteRefInstance(UniversalObjectInterface *initialResultObject,
        TransformProcessor *initialParentTransformer,
        TimelineKey *initialKey,
        FileReference *initialImageRef,
        bool initialUseDefaultPivotPoint) :
        ObjectRefInstance(initialResultObject, initialParentTransformer, initialKey),
        imageRef(initialImageRef),
        useDefaultPivot(initialUseDefaultPivotPoint)
    {
        // if (imageRef == nullptr)
        //    std::cerr << "[Spriter++] Initializing with nullptr imageRef, that's suspicious" << std::endl;
    }

    void SpriteRefInstance::process(real currentTime)
    {
        if (imageRef == nullptr)
            return;

        ObjectRefInstance::process(currentTime);
        resultObject()->setImage(imageRef->image());
        if (useDefaultPivot && imageRef->image())
        {
            resultObject()->setPivot(imageRef->image()->defaultPivot);
        }
    }

}
