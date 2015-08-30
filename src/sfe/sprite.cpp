#include "sfe/sprite.hpp"

#include "core/application.hpp"
#include "tools/vector.hpp"

using namespace sfe;

Sprite::Sprite()
{
    setDetectable(false);

    addPart(&m_sprite);
}

//--------------------//
//----- Wrappers -----//

void Sprite::setTexture(const std::string& textureID)
{
    const auto& texture = Application::context().textures.get(textureID);
    m_sprite.setTexture(texture);
    setSize(sf::v2f(texture.getSize()));
}
