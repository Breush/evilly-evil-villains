#include "ai/dumbpuppet.hpp"

#include "scene/components/lerpable.hpp"
#include "tools/random.hpp"
#include "tools/tools.hpp"

using namespace ai;

DumbPuppet::DumbPuppet()
{
    addComponent<scene::Lerpable>();

    attachChild(m_sprite);
}

//-------------------//
//----- Routine -----//

void DumbPuppet::updateAI(const sf::Time&)
{
    auto lerpable = getComponent<scene::Lerpable>();

    returnif (lerpable->positionLerping());

    // Go somethere randomly
    sf::Vector2f targetPosition(alea::rand(m_minX, m_maxX), localPosition().y);
    lerpable->setTargetPosition(targetPosition);

    if (localPosition().x < targetPosition.x)
        m_sprite.select("rwalk");
    else
        m_sprite.select("lwalk");
}

//-------------------//
//----- Control -----//

void DumbPuppet::setSource(const std::string& source)
{
    m_sprite.load(source);
}

void DumbPuppet::setSpeed(const sf::Vector2f& speed)
{
    getComponent<scene::Lerpable>()->setPositionSpeed(speed);
}

void DumbPuppet::setInitialLocalPosition(const sf::Vector2f& position)
{
    setLocalPosition(position);
    getComponent<scene::Lerpable>()->setTargetPosition(position);
}

void DumbPuppet::setHorizontalRange(float minX, float maxX)
{
    m_minX = minX;
    m_maxX = maxX;
}
