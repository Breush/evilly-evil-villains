#include "spriter/exampleobjectfactory.hpp"

#include "spriter/sfmlpointobjectinfo.hpp"
#include "spriter/sfmlboxinstanceinfo.hpp"
#include "spriter/sfmlsoundobjectinforeference.hpp"

namespace SpriterEngine
{

    ExampleObjectFactory::ExampleObjectFactory(sf::RenderWindow *validRenderWindow) :
        renderWindow(validRenderWindow)
    {
    }

    PointObjectInfo * ExampleObjectFactory::newPointObjectInfo()
    {
        return new SfmlPointObjectInfo(renderWindow);
    }

    BoxInstanceInfo * ExampleObjectFactory::newBoxInstanceInfo(point size)
    {
        return new SfmlBoxInstanceInfo(size, renderWindow);
    }

}
