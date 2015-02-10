#include "sfe/lerpable.hpp"

#include "tools/tools.hpp"

using namespace sfe;

Lerpable::Lerpable()
    : m_lerpable(false)
    , m_lerpablePositionSpeed(250.f, 250.f)
{
}

//-------------------//
//----- Routine -----//

void Lerpable::update(const sf::Time& dt)
{
    baseClass::update(dt);
    returnif (!lerpable());

    // Updating position
    if (localPosition() != m_targetPosition)
        setLocalPosition(nextPosition(dt));

    // TODO Other interpolations
}

//---------------------------//
//----- Target relative -----//

void Lerpable::setTargetPositionOffset(const sf::Vector2f& positionOffset)
{
    setTargetPosition(defaultPosition() + positionOffset);
}

//--------------------------//
//----- Interpolations -----//

sf::Vector2f Lerpable::nextPosition(const sf::Time& dt)
{
    sf::Vector2f position(localPosition());
    sf::Vector2f offset(m_lerpablePositionSpeed * dt.asSeconds());

    converge(position.x, m_targetPosition.x, offset.x);
    converge(position.y, m_targetPosition.y, offset.y);

    return position;
}

//-------------------//
//----- Changes -----//

void Lerpable::saveDefaults()
{
    m_defaultPosition = localPosition();
    m_targetPosition = localPosition();
}

void Lerpable::changedTargetPosition()
{
}

//---------------------//
//----- Low level -----//

void Lerpable::converge(float& source, const float target, const float offset)
{
    if (source < target) {
        source += offset;
        if (source > target)
            source = target;
    }
    else if (source > target) {
        source -= offset;
        if (source < target)
            source = target;
    }
}
