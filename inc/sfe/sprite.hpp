#pragma once

#include "scene/entity.hpp"

#include <SFML/Graphics/Sprite.hpp>

namespace sfe
{
    /** sfe::Sprite
     *      Just a wrapper of sf::Sprite to a scene::Entity
     **/

    class Sprite : public scene::Entity
    {
        using baseClass = scene::Entity;

    public:
        Sprite();
        virtual ~Sprite() {}

        void setTexture(Textures::ID textureID);

    protected:
        void update() override;

    private:
        sf::Sprite m_sprite;
    };
}
