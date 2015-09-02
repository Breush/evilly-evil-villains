#pragma once

#include "tools/param.hpp"

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/VertexArray.hpp>

namespace sfe
{
    //! Abstract class to display a simple arrow.

    class Arrow : public sf::Drawable, public sf::Transformable
    {
    public:

        //! Constructor.
        Arrow();

        //! Default destructor.
        virtual ~Arrow() = default;

        //----------------//
        //! @name Routine
        //! @{

        //! Implements sf::Drawable routine.
        void draw(sf::RenderTarget& target, sf::RenderStates states) const final;

        //! @}

        //--------------------------//
        //! @name Public properties
        //! @{

        //! Size override.
        PARAMGSU(float, m_length, length, setLength, updateFromLength)

        //! @}

    protected:

        //----------------//
        //! @name Updates
        //! @{

        //! Abstract method to recompute vertices position when length changed.
        virtual void updateFromLength() = 0;

        //! @}

    protected:

        //! An arrow is just 4 vertices.
        sf::VertexArray m_vertices;
    };

    //! An sf::Drawable arrow going left.

    class LArrow final : public Arrow
    {
    public:

        //! Default constructor.
        LArrow() = default;

        //! Default destructor.
        ~LArrow() = default;

    protected:

        //----------------//
        //! @name Updates
        //! @{

        void updateFromLength() final;

        //! @}
    };

    //! An sf::Drawable arrow going right.

    class RArrow final : public Arrow
    {
    public:

        //! Default constructor.
        RArrow() = default;

        //! Default destructor.
        ~RArrow() = default;

    protected:

        //----------------//
        //! @name Updates
        //! @{

        void updateFromLength() final;

        //! @}
    };
}
