#include "nui/rectangleshape.hpp"

#include "context/context.hpp"

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
}

void RectangleShape::onScaleChanges()
{
    m_rectangleShape.setScale(scale());
}

//--------------------//
//----- Wrappers -----//

void RectangleShape::setTexture(const std::string& textureID)
{
    m_rectangleShape.setTexture(&context::context.textures.get(textureID));
}
