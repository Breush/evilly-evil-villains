#include "spriter/spriterobjectfactory.hpp"

#include "spriter/sfmlpointinstanceinfo.hpp"
#include "spriter/sfmlboxinstanceinfo.hpp"
#include "spriter/sfmlsoundobjectinforeference.hpp"

using namespace SpriterEngine;

SpriterObjectFactory::SpriterObjectFactory()
{
}

//--------------------//
//----- Wrappers -----//

PointInstanceInfo* SpriterObjectFactory::newPointInstanceInfo()
{
    return new SfmlPointInstanceInfo();
}

BoxInstanceInfo* SpriterObjectFactory::newBoxInstanceInfo(point size)
{
    return new SfmlBoxInstanceInfo(size);
}
