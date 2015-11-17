#pragma once

#include "tools/param.hpp"

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/VertexArray.hpp>

namespace sfe
{
    //! Abstract class to display a simple line.

    class Line : public sf::Drawable, public sf::Transformable
    {
    public:

        //! Constructor.
        Line();

        //! Default destructor.
        virtual ~Line() = default;

        //----------------//
        //! @name Routine
        //! @{

        //! Implements sf::Drawable drawing routine.
        void draw(sf::RenderTarget& target, sf::RenderStates states) const final;

        //! @}

        //----------------//
        //! @name Control
        //! @{

        //! Set line color.
        void setColor(const sf::Color& color);

        //! @}

        //--------------------------//
        //! @name Public properties
        //! @{

        //! Size override.
        PARAMGSU(float, m_length, length, setLength, update)

        //! Percent of line fading away in borders.
        PARAMGSU(float, m_shade, shade, setShade, update)

        //! @}

    protected:

        //----------------//
        //! @name Updates
        //! @{

        //! Update vertices positions.
        virtual void update() = 0;

        //! @}

    protected:

        //! A line is just 4 vertices.
        sf::VertexArray m_vertices;
    };

    //! An horizontal sf::Drawable line.

    class HLine final : public Line
    {
    public:

        //! Default constructor.
        HLine() = default;

        //! Default destructor.
        ~HLine() = default;

    protected:

        //----------------//
        //! @name Updates
        //! @{

        void update() final;

        //! @}
    };

    //! A vertical sf::Drawable line.

    class VLine final : public Line
    {
    public:

        //! Default constructor.
        VLine() = default;

        //! Default destructor.
        ~VLine() = default;

    protected:

        //----------------//
        //! @name Updates
        //! @{

        void update() final;

        //! @}
    };
}

