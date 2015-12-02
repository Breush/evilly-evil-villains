#include "spriter/sfmlboxinstanceinfo.hpp"

#include <Spriter/global/settings.h>

using namespace SpriterEngine;

SfmlBoxInstanceInfo::SfmlBoxInstanceInfo(point initialSize)
    : BoxInstanceInfo(initialSize)
    , rectangle({static_cast<float>(initialSize.x), static_cast<float>(initialSize.y)})
{
}

void SfmlBoxInstanceInfo::render(sf::RenderTarget& target, sf::RenderStates& states, const sf::Color&)
{
    if (Settings::renderDebugBoxes)
    {
        rectangle.setPosition(getPosition().x, getPosition().y);
        rectangle.setRotation(toDegrees(getAngle()));
        rectangle.setScale(getScale().x, getScale().y);
        rectangle.setOrigin(getPivot().x*getSize().x, getPivot().y*getSize().y);
        target.draw(rectangle, states);
    }
}
