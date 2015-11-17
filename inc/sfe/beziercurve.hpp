#pragma once

#include "tools/int.hpp"

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/VertexArray.hpp>

namespace sfe
{
    //! Class to display a cubic Bézier curve.

    class BezierCurve : public sf::Drawable, public sf::Transformable
    {
    public:

        //! Constructor.
        BezierCurve();

        //! Default destructor.
        virtual ~BezierCurve() = default;

        //----------------//
        //! @name Routine
        //! @{

        //! Implements sf::Drawable drawing routine.
        void draw(sf::RenderTarget& target, sf::RenderStates states) const final;

        //! @}

        //----------------//
        //! @name Control
        //! @{

        //! Set curve parameters.
        void set(const sf::Vector2f& s1, const sf::Vector2f& s2, const sf::Vector2f& c1, const sf::Vector2f& c2);

        //! Set curve color.
        void setColor(const sf::Color& color);

        //! @}

    protected:

        //! A curve is just a bunch of vertices.
        sf::VertexArray m_vertices;
    };
}

