#pragma once

#include "tools/param.hpp"

#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>

// Forward declarations
namespace scene
{
    class Entity;
}

namespace sfe
{
    class Lerpable
    {
    public:
        Lerpable(scene::Entity* entity);
        virtual ~Lerpable() = default;

        // Virtual
        void saveDefaults();
        void update(const sf::Time& dt);

        // Targets relative
        void setTargetPositionOffset(const sf::Vector2f& positionOffset);

    protected:
        // Changes
        void changedTargetPosition();

        // Interpolations
        sf::Vector2f nextPosition(sf::Vector2f position, const sf::Time& dt);

        // Low level
        void converge(float& source, const float target, const float offset);

        // Params
        PARAMGS(sf::Vector2f, m_positionSpeed, positionSpeed, setPositionSpeed)
        PARAMGSU(sf::Vector2f, m_targetPosition, targetPosition, setTargetPosition, changedTargetPosition)
        PARAMG(sf::Vector2f, m_defaultPosition, defaultPosition)

    private:
        scene::Entity* m_entity;
    };
}

