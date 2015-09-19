#pragma once

#include "scene/entity.hpp"
#include "tools/int.hpp"

#include <SFML/Graphics/Sprite.hpp>

namespace scene
{
    //! Wrapper of sf::Sprite as a scene::Entity.

    class Sprite final : public Entity
    {
        using baseClass = Entity;

    public:

        //! Constructor.
        Sprite();

        //! Default destructor.
        ~Sprite() = default;

        //-----------------//
        //! @name Wrappers
        //! @{

        //! Wrapper to set a texture.
        void setTexture(const std::string& textureID);

        //! @}

    private:

        //! The sprite itself.
        sf::Sprite m_sprite;
    };
}
