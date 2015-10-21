#include "nui/rectangleshape.hpp"

#include "core/application.hpp"

using namespace nui;

RectangleShape::RectangleShape()
{
    addPart(&m_rectangleShape);
}

//-------------------//
//----- Routine -----//

void RectangleShape::onSizeChanges()
{
    m_rectangleShape.setSize(size());
    setScale(scale());
}

//--------------------//
//----- Wrappers -----//

void RectangleShape::setTexture(const std::string& textureID)
{
    m_rectangleShape.setTexture(&Application::context().textures.get(textureID));
}
