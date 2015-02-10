#pragma once

#include "interaction/detectable.hpp"

#include <SFML/System/Time.hpp>

namespace sfe
{
    class Lerpable : public interaction::Detectable
    {
        typedef interaction::Detectable baseClass;

    public:
        Lerpable();
        virtual ~Lerpable() {}

        // Virtual
        void saveDefaults();
        void update(const sf::Time& dt) override;

        // Targets relative
        void setTargetPositionOffset(const sf::Vector2f& positionOffset);

    protected:
        // Changes
        void changedTargetPosition();

        // Interpolations
        sf::Vector2f nextPosition(const sf::Time& dt);

        // Low level
        void converge(float& source, const float target, const float offset);

        // Params
        PARAMGS(bool, m_lerpable, lerpable, setLerpable)
        PARAMGS(sf::Vector2f, m_lerpablePositionSpeed, lerpablePositionSpeed, setLerpablePositionSpeed)
        PARAMGSU(sf::Vector2f, m_targetPosition, targetPosition, setTargetPosition, changedTargetPosition)
        PARAMG(sf::Vector2f, m_defaultPosition, defaultPosition)
    };
}

