#include "spriter/exampleobjectfactory.hpp"

#include "spriter/sfmlpointinstanceinfo.hpp"
#include "spriter/sfmlboxinstanceinfo.hpp"
#include "spriter/sfmlsoundobjectinforeference.hpp"

namespace SpriterEngine
{

    ExampleObjectFactory::ExampleObjectFactory(sf::RenderWindow *validRenderWindow) :
        renderWindow(validRenderWindow)
    {
    }

    PointInstanceInfo * ExampleObjectFactory::newPointInstanceInfo()
    {
        return new SfmlPointInstanceInfo(renderWindow);
    }

    BoxInstanceInfo * ExampleObjectFactory::newBoxInstanceInfo(point size)
    {
        return new SfmlBoxInstanceInfo(size, renderWindow);
    }

}
