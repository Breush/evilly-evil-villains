#include "spriter/sfmlpointinstanceinfo.hpp"

#include <Spriter/global/settings.h>

namespace SpriterEngine
{

    SfmlPointInstanceInfo::SfmlPointInstanceInfo(sf::RenderWindow *validRenderWindow) :
        renderWindow(validRenderWindow),
        circle(10)
    {
        circle.setFillColor(sf::Color::Red);
        circle.setOrigin(5, 5);
    }

    void SfmlPointInstanceInfo::render()
    {
        if (Settings::renderDebugPoints)
        {
            circle.setPosition(getPosition().x, getPosition().y);
            circle.setRotation(toDegrees(getAngle()));
            renderWindow->draw(circle);
        }
    }

}
