#pragma once

#include "tools/param.hpp"

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>

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

        //! Implements sf::Drawable drawing routine.
        void draw(sf::RenderTarget& target, sf::RenderStates states) const final;

        //! @}

        //----------------//
        //! @name Control
        //! @{

        //! Size override.
        void setLength(float inLength);

        //! @}

        //--------------------------//
        //! @name Public properties
        //! @{

        //! Size override.
        PARAMG(float, m_length, length)

        //! @}

    protected:

        //----------------//
        //! @name Updates
        //! @{

        //! Abstract method to recompute vertices position when length changed.
        virtual void updateFromLength() = 0;

        //! Refresh the target texture.
        void refreshRenderTexture();

        //! @}

    protected:

        //! An arrow is just 4 vertices.
        sf::VertexArray m_vertices;

        //! Drawn to a texture to be able to use a shader.
        sf::RenderTexture m_renderTexture;

        //! The displayed part.
        sf::Sprite m_sprite;
    };

    //! An sf::Drawable arrow going left.

    class LArrow final : public Arrow
    {
        using baseClass = Arrow;

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

    //! A sf::Drawable arrow going right.

    class RArrow final : public Arrow
    {
        using baseClass = Arrow;

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
