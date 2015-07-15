#include "sfe/rectangleshape.hpp"

#include "core/application.hpp"
#include "resources/identifiers.hpp"

using namespace sfe;

RectangleShape::RectangleShape()
{
    setDetectable(false);

    addPart(&m_rectangleShape);
}

//--------------------//
//----- Wrappers -----//

void RectangleShape::setTexture(TextureID textureID)
{
    m_rectangleShape.setTexture(&Application::context().textures.get(textureID));
}
