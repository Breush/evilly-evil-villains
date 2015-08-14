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
    // TODO Use source
    m_sprite.setFillColor(sf::Color::White);
    m_sprite.setSize({30.f, 30.f});
    setOrigin({15.f, 15.f});
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
