#pragma once

#include "scene/entity.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

namespace scene
{
    //! An entity reacting to light.
    //! Can have a normals map.

    class LightSprite final : public scene::Entity
    {
    public:

        //! Constructor.
        LightSprite();

        //! Destructor.
        ~LightSprite();

        std::string _name() const noexcept final { return "scene::LightSprite"; }

        //----------------//
        //! @name Control
        //! @{

        //! Get the current texture.
        inline const sf::Texture* texture() const { return m_shape.getTexture(); }

        //! Set a texture.
        //! If there exists a similar textureID with _NORMALS postfix, it will be used as the normals texture.
        void setTexture(const std::string& textureID);

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onSizeChanges() final;
        void onScaleChanges() final;

        //! @}

    private:

        //! The rectangle shape itself.
        sf::RectangleShape m_shape;
    };
}
