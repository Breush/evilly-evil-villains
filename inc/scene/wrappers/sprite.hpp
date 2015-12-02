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

        std::string _name() const final { return "Sprite"; }

        //-----------------//
        //! @name Wrappers
        //! @{

        //! Wrapper to set a texture.
        void setTexture(const std::string& textureID);

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onScaleChanges() final;

        //! @}

    private:

        //! The sprite itself.
        sf::Sprite m_sprite;
    };
}
