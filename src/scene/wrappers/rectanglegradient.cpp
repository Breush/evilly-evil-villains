#include "scene/wrappers/rectanglegradient.hpp"

using namespace scene;

RectangleGradient::RectangleGradient()
{
    setDetectable(false);

    addPart(&m_rectangleGradient);
}

//-------------------//
//----- Routine -----//

void RectangleGradient::onSizeChanges()
{
    m_rectangleGradient.setSize(size());
    setScale(scale());
}
