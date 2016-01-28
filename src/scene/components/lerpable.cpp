#include "scene/components/lerpable.hpp"

#include "scene/entity.hpp"
#include "tools/tools.hpp"

using namespace scene;

Lerpable::Lerpable(Entity& entity)
    : baseClass(entity)
    , m_positionSpeed(250.f, 250.f)
{
}

//-------------------//
//----- Routine -----//

void Lerpable::update(const sf::Time& dt)
{
    // Updating position
    if (m_positionLerping) {
        const auto& localPosition(m_entity.localPosition());
        m_entity.setLocalPosition(nextPosition(localPosition, dt));
        if (m_entity.localPosition() == m_targetPosition)
            m_positionLerping = false;
    }

    // TODO Other variables
}

//--------------------------//
//----- User interface -----//

void Lerpable::saveDefaults()
{
    m_defaultPosition = m_entity.localPosition();
}

//----------------------------------//
//----- Position interpolation -----//

void Lerpable::setTargetPosition(const sf::Vector2f& targetPosition)
{
    m_targetPosition = targetPosition;
    m_positionLerping = true;
}

void Lerpable::setTargetPositionOffset(const sf::Vector2f& positionOffset)
{
    setTargetPosition(m_defaultPosition + positionOffset);
}

//--------------------------//
//----- Interpolations -----//

sf::Vector2f Lerpable::nextPosition(sf::Vector2f position, const sf::Time& dt)
{
    sf::Vector2f offset(m_positionSpeed * dt.asSeconds());

    converge(position.x, m_targetPosition.x, offset.x);
    converge(position.y, m_targetPosition.y, offset.y);

    return position;
}

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
