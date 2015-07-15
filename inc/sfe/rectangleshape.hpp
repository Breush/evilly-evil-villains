#pragma once

#include "scene/entity.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

// Forward declarations

enum class TextureID : uint8;

namespace sfe
{
    //! Wrapper of sf::RectangleShape as a scene::Entity.

    class RectangleShape final : public scene::Entity
    {
        using baseClass = scene::Entity;

    public:

        //! Constructor.
        RectangleShape();

        //! Default destructor.
        ~RectangleShape() = default;

        //-----------------//
        //! @name Wrappers
        //! @{

        //! Wrapper to set a texture.
        void setTexture(TextureID textureID);

        //! Wrapper to set the fill color.
        inline void setFillColor(const sf::Color& color)
        {
            m_rectangleShape.setFillColor(color);
        }

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onSizeChanges() final
        {
            m_rectangleShape.setSize(size());
        }

        //! @}

    private:

        //! The rectangle shape itself.
        sf::RectangleShape m_rectangleShape;
    };
}
