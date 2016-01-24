#include "scene/wrappers/sprite3d.hpp"

#include "context/context.hpp"
#include "tools/vector.hpp"

using namespace scene;

Sprite3D::Sprite3D()
{
    setDetectable(false);

    addPart(&m_sprite);
}

//-------------------//
//----- Routine -----//

void Sprite3D::onScaleChanges()
{
    m_sprite.setScale(scale());
}

//--------------------//
//----- Wrappers -----//

void Sprite3D::setTexture(const std::string& textureID)
{
    const auto& texture = context::context.textures.get(textureID);
    m_sprite.setTexture(texture);

    auto textureSize = sf::v2f(texture.getSize());
    setSize(textureSize);
}

void Sprite3D::setOrigin3D(const sf::Vector3f& origin3D)
{
    m_sprite.setOrigin3D(origin3D);

    // We move the sprite as this also changes the origin of the transformable
    m_sprite.setPosition(origin3D.x, origin3D.y);
}
