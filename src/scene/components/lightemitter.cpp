#include "scene/components/lightemitter.hpp"

using namespace scene;

LightEmitter::LightEmitter(Entity& entity)
    : baseClass(entity)
{
}

LightEmitter::~LightEmitter()
{
}

//---------------------//
//----- Callbacks -----//

void LightEmitter::onLayerChanged()
{
    // TODO
}

//----------------------------//
//----- Lighting control -----//

uint32 LightEmitter::addPoint(const sf::Vector2f& relPos, float lightSize)
{
    // TODO
    return 0u;
}

void LightEmitter::setColor(uint32 lightID, const sf::Color& color)
{
    // TODO
}
