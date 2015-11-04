#pragma once

#include <Spriter/override/objectfactory.h>

#include <SFML/Graphics/RenderWindow.hpp>

namespace SpriterEngine
{
    class ExampleObjectFactory : public ObjectFactory
    {
    public:

        ExampleObjectFactory(sf::RenderWindow *validRenderWindow);

        PointObjectInfo *newPointObjectInfo() override;

        BoxInstanceInfo *newBoxInstanceInfo(point size) override;

    private:

        sf::RenderWindow *renderWindow;
    };

}
