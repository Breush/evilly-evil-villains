#pragma once

#include "scene/entity.hpp"
#include "scene/wrappers/animatedsprite.hpp"

// TODO Why is that in AI?
// Should be in sfe maybe

namespace ai
{
    //! Dumb puppet that moves inside specific coordinates.

    class DumbPuppet final : public scene::Entity
    {
        using baseClass = scene::Entity;

    public:

        //! Constructor.
        DumbPuppet();

        //! Default destructor.
        ~DumbPuppet() = default;

        std::string _name() const final { return "ai::DumbPuppet"; }

        //----------------//
        //! @name Control
        //! @{

        //! Set the visual part.
        void setSource(const std::string& source);

        //! Set the initial local position.
        void setInitialLocalPosition(const sf::Vector2f& position);

        //! Set the horizontal range.
        void setHorizontalRange(float minX, float maxX);

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void updateAI(const sf::Time& dt);

        //! @}

    private:

        float m_minX = -1.f;    //!< Mininum horizontal local position.
        float m_maxX = -1.f;    //!< Maximum horizontal local position.

        scene::AnimatedSprite m_sprite;   //!< The sprite.
    };
}
