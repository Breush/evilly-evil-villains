#pragma once

#include "scene/entity.hpp"
#include "sfe/beziercurve.hpp"

namespace scene
{
    //! A customizable line over a scene::Entity.

    class CustomLine final : public Entity
    {
        using baseClass = Entity;

    public:

        //! Constructor.
        CustomLine();

        //! Default destructor.
        ~CustomLine() = default;

        std::string _name() const final { return "CustomLine"; }

        //----------------//
        //! @name Control
        //! @{

        //! The local positions of two points of the custom line.
        void setLimits(const sf::Vector2f& a, const sf::Vector2f& b);

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void updateRoutine(const sf::Time& dt);

        //! @}

        //------------//
        //! @name ICU
        //! @{

        //! Refresh the curve display (recompute points positions).
        void refreshCurve();

        //! @}

    private:

        //! The curve used.
        sfe::BezierCurve m_curve;

        float m_time = 0.f;     //!< How much time has passed.
        float m_tFactor = 0.5f; //!< Line waviness (0.f = straigth).

        sf::Vector2f m_a;   //!< Starting point.
        sf::Vector2f m_b;   //!< End point.
        sf::Vector2f m_d;   //!< Pre-computed direction (b - a).
        sf::Vector2f m_t;   //!< Pre-computed anti-direction scalarProduct(d, t) = 0.
    };
}
