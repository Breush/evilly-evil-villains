#pragma once

#include "scene/entity.hpp"
#include "tools/int.hpp"

#include <SFML/Graphics/Sprite.hpp>

// Forward declarations

enum class TextureID : uint8;

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
        virtual ~Sprite() = default;

        void setTexture(TextureID textureID);

    protected:
        void update() override;

    private:
        sf::Sprite m_sprite;
    };
}
