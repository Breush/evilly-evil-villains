#include "ai/dumbpuppet.hpp"

#include "tools/tools.hpp"
#include "tools/random.hpp"

using namespace ai;

DumbPuppet::DumbPuppet()
    : baseClass(true)
{
    attachChild(m_sprite);
}

//-------------------//
//----- Routine -----//

void DumbPuppet::updateAI(const sf::Time&)
{
    returnif (lerpable()->positionLerping());

    // Go somethere randomly
    lerpable()->setTargetPosition({alea::rand(m_minX, m_maxX), localPosition().y});
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
}
