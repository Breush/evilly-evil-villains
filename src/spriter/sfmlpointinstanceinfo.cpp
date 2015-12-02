#include "spriter/sfmlpointinstanceinfo.hpp"

#include <Spriter/global/settings.h>

using namespace SpriterEngine;

SfmlPointInstanceInfo::SfmlPointInstanceInfo()
    : circle(10)
{
    circle.setFillColor(sf::Color::Red);
    circle.setOrigin(5, 5);
}

void SfmlPointInstanceInfo::render(sf::RenderTarget& target, sf::RenderStates& states, const sf::Color&)
{
    if (Settings::renderDebugPoints)
    {
        circle.setPosition(getPosition().x, getPosition().y);
        circle.setRotation(toDegrees(getAngle()));
        target.draw(circle, states);
    }
}
