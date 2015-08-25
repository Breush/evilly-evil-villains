#include "ai/dumbpuppet.hpp"

#include "tools/tools.hpp"

using namespace ai;

DumbPuppet::DumbPuppet()
    : baseClass(true)
{
    attachChild(m_sprite);
}

//-------------------//
//----- Routine -----//

void DumbPuppet::updateAI(const sf::Time& dt)
{
    returnif (lerpable()->positionLerping());

    if (localPosition().x == m_minX)    lerpable()->setTargetPosition({m_maxX, localPosition().y});
    else                                lerpable()->setTargetPosition({m_minX, localPosition().y});
}

//-------------------//
//----- Control -----//

void DumbPuppet::setSource(const std::string& source)
{
    m_sprite.load(source);
}

void DumbPuppet::setInitialLocalPosition(const sf::Vector2f& position)
{
    setLocalPosition(position);
    lerpable()->setTargetPosition(position);
}

void DumbPuppet::setHorizontalRange(float minX, float maxX)
{
    m_minX = minX;
    m_maxX = maxX;

    lerpable()->setTargetPosition({m_minX, localPosition().y});
}
