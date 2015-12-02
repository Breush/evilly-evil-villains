#pragma once

#include <Spriter/override/objectfactory.h>

namespace SpriterEngine
{
    //! Object factory based on SpriterPlusPlus interface.

    class SpriterObjectFactory final : public ObjectFactory
    {
    public:

        //! Constructor.
        SpriterObjectFactory();

        //-----------------//
        //! @name Wrappers
        //! @{

        PointInstanceInfo* newPointInstanceInfo() final;
        BoxInstanceInfo* newBoxInstanceInfo(point size) final;

        //! @}
    };
}
