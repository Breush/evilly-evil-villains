#pragma once

#include <SFML/Graphics/RenderWindow.hpp>

#include <Spriter/override/objectfactory.h>

namespace SpriterEngine
{
    class ExampleObjectFactory : public ObjectFactory
    {
    public:
        ExampleObjectFactory(sf::RenderWindow *validRenderWindow);

        PointInstanceInfo *newPointInstanceInfo() override;

        BoxInstanceInfo *newBoxInstanceInfo(point size) override;

    private:
        sf::RenderWindow *renderWindow;
    };
}
