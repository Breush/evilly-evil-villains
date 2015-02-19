#include "sfe/sprite.hpp"

#include "core/application.hpp"
#include "resources/identifiers.hpp"

using namespace sfe;

Sprite::Sprite()
{
    addPart(&m_sprite);
}

void Sprite::update()
{
    m_sprite.setScale(localScale());
}

void Sprite::setTexture(TextureID textureID)
{
    m_sprite.setTexture(Application::context().textures.get(textureID));
}
