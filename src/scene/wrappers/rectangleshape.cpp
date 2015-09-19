#include "scene/wrappers/rectangleshape.hpp"

#include "core/application.hpp"

using namespace scene;

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
