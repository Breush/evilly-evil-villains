#include "sfe/sprite.hpp"

#include "core/application.hpp"

using namespace sfe;

Sprite::Sprite()
{
    addPart(&m_sprite);
}

void Sprite::update()
{
    m_sprite.setScale(localScale());
}

void Sprite::setTexture(Textures::ID textureID)
{
    m_sprite.setTexture(Application::context().textures.get(textureID));
}
