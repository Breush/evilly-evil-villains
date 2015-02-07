#pragma once

#include "tools/param.hpp"

#include <SFML/Graphics/Transformable.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Time.hpp>

namespace sfe
{
    class LocalTransformable : public sf::Transformable
    {
    public:
        LocalTransformable();
        virtual ~LocalTransformable() {}

        virtual void update(const sf::Time& dt);
        void refresh();

        // Setters
        void setLocalPosition(const sf::Vector2f& position, bool inCallParent = true);
        void setLocalRotation(const float& rotation, bool inCallParent = true);
        void setLocalScale(const sf::Vector2f& scaling, bool inCallParent = true);

        // Actions
        void localMove(const sf::Vector2f& position);
        void localRotate(const float& rotation);
        void localScale(const sf::Vector2f& scaling);

    protected:
        // Changes
        virtual void changedStatus();
        void changedParent();
        void changedLocalPosition();
        void changedLocalRotation();
        void changedLocalScale();

        // Updates
        void updateLocal();
        void updateLocalPosition();
        void updateLocalRotation();
        void updateLocalScale();

        // Parent calls
        virtual void childChangedStatus(LocalTransformable* child) {}

        // Params
        PARAMGS(bool, m_callParent, callParent, setCallParent)
        PARAMGSU(bool, m_status, status, setStatus, changedStatus)
        PARAMGSU(LocalTransformable*, m_parent, parent, setParent, changedParent)
        PARAMG(sf::Vector2f, m_localPosition, localPosition)
        PARAMG(float, m_localRotation, localRotation)
        PARAMG(sf::Vector2f, m_localScale, localScale)
    };
}
