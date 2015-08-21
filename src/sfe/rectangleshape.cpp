#include "sfe/rectangleshape.hpp"

#include "core/application.hpp"

using namespace sfe;

RectangleShape::RectangleShape()
{
    setDetectable(false);

    addPart(&m_rectangleShape);
}

//--------------------//
//----- Wrappers -----//

void RectangleShape::setTexture(const std::string& textureID)
{
    m_rectangleShape.setTexture(&Application::context().textures.get(textureID));
}
