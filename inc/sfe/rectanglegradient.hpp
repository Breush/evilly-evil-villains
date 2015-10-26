#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/VertexArray.hpp>

namespace sfe
{
    //! Displays a simple colored rectangle with gradient.

    class RectangleGradient final : public sf::Drawable, public sf::Transformable
    {
    public:

        //! Corner point position.
        enum PointPosition
        {
            TOP_LEFT,
            TOP_RIGHT,
            BOTTOM_LEFT,
            BOTTOM_RIGHT,
        };

    public:

        //! Constructor.
        RectangleGradient();

        //----------------//
        //! @name Control
        //! @{

        //! Set the size of the rectangle.
        void setSize(const sf::Vector2f& size);

        //! Set a specific point color.
        void setPointColor(const PointPosition pointPosition, const sf::Color& color);

        //! Set the top and bottom colors.
        void setPointsColors(const sf::Color& topColor, const sf::Color& bottomColor);

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        //! Implements sf::Drawable drawing routine.
        void draw(sf::RenderTarget& target, sf::RenderStates states) const final;

        //! @}

    private:

        //! A rectangle is just 4 vertices.
        sf::VertexArray m_vertices;
    };
}
