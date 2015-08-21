#pragma once

#include "scene/entity.hpp"
#include "tools/int.hpp"

#include <SFML/Graphics/Sprite.hpp>

namespace sfe
{
    /** sfe::Sprite
     *      Just a wrapper of sf::Sprite to a scene::Entity
     **/

    // TODO DOC

    class Sprite final : public scene::Entity
    {
        using baseClass = scene::Entity;

    public:

        Sprite();
        ~Sprite() = default;

        void setTexture(const std::string& textureID);

    private:
        sf::Sprite m_sprite;
    };
}
