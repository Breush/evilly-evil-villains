#include "sfe/lerpable.hpp"

#include "scene/entity.hpp"
#include "tools/debug.hpp"
#include "tools/tools.hpp"

using namespace sfe;

Lerpable::Lerpable(scene::Entity* entity)
    : m_positionSpeed(250.f, 250.f)
    , m_entity(entity)
{
    massert(m_entity != nullptr, "Init lerpable with empty entity.");
}

//-------------------//
//----- Routine -----//

void Lerpable::update(const sf::Time& dt)
{
    const auto& localPosition(m_entity->localPosition());

    // Updating position
    if (localPosition != m_targetPosition)
        m_entity->setLocalPosition(nextPosition(localPosition, dt));

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

sf::Vector2f Lerpable::nextPosition(sf::Vector2f position, const sf::Time& dt)
{
    sf::Vector2f offset(m_positionSpeed * dt.asSeconds());

    converge(position.x, m_targetPosition.x, offset.x);
    converge(position.y, m_targetPosition.y, offset.y);

    return position;
}

//-------------------//
//----- Changes -----//

void Lerpable::saveDefaults()
{
    m_defaultPosition = m_entity->localPosition();
    m_targetPosition = m_entity->localPosition();
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
