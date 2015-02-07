#include "sfe/localtransformable.hpp"

using namespace sfe;

LocalTransformable::LocalTransformable()
    : m_status(true)
    , m_callParent(true)
    , m_parent(nullptr)
    , m_localPosition()
    , m_localRotation(0.f)
    , m_localScale(1.f, 1.f)
{
}

void LocalTransformable::update(const sf::Time&)
{
    if (m_status) {
        updateLocal();

        if (m_callParent && m_parent != nullptr)
            m_parent->childChangedStatus(this);

        setStatus(false);
    }

    setCallParent(true);
}

void LocalTransformable::refresh()
{
    setStatus(true);
    setCallParent(true);
}

//-------------------//
//----- Setters -----//

void LocalTransformable::setLocalPosition(const sf::Vector2f& position, bool inCallParent)
{
    m_localPosition = position;
    if (!inCallParent) setCallParent(false);
    changedLocalPosition();
}

void LocalTransformable::setLocalRotation(const float& rotation, bool inCallParent)
{
    m_localRotation = rotation;
    if (!inCallParent) setCallParent(false);
    changedLocalRotation();
}

void LocalTransformable::setLocalScale(const sf::Vector2f& scaling, bool inCallParent)
{
    m_localScale = scaling;
    if (!inCallParent) setCallParent(false);
    changedLocalScale();
}

//-------------------//
//----- Actions -----//

void LocalTransformable::localMove(const sf::Vector2f& position)
{
    setLocalPosition(m_localPosition + position);
}

void LocalTransformable::localRotate(const float& rotation)
{
    setLocalRotation(m_localRotation + rotation);
}

void LocalTransformable::localScale(const sf::Vector2f& scaling)
{
    setLocalScale(m_localScale + scaling);
}

//-------------------//
//----- Updates -----//

void LocalTransformable::updateLocal()
{
    updateLocalPosition();
    updateLocalRotation();
    updateLocalScale();
}

void LocalTransformable::updateLocalPosition()
{
    if (m_parent) {
        setPosition(m_parent->getPosition());
        move(m_localPosition);
    } else {
        setPosition(m_localPosition);
    }
}

void LocalTransformable::updateLocalRotation()
{
    if (m_parent) {
        setRotation(m_parent->getRotation());
        rotate(m_localRotation);
    } else {
        setRotation(m_localRotation);
    }
}

void LocalTransformable::updateLocalScale()
{
    if (m_parent) {
        setScale(m_parent->getScale());
        scale(m_localScale);
    } else {
        setScale(m_localScale);
    }
}

//-------------------//
//----- Changes -----//

void LocalTransformable::changedStatus()
{
}

void LocalTransformable::changedParent()
{
    updateLocal();
    setStatus(true);
}

void LocalTransformable::changedLocalPosition()
{
    updateLocalPosition();
    setStatus(true);
}

void LocalTransformable::changedLocalRotation()
{
    updateLocalRotation();
    setStatus(true);
}

void LocalTransformable::changedLocalScale()
{
    updateLocalScale();
    setStatus(true);
}
