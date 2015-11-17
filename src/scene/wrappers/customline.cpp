#include "scene/wrappers/customline.hpp"

#include "tools/math.hpp"
#include "tools/vector.hpp"

using namespace scene;

CustomLine::CustomLine()
{
    setDetectable(false);

    addPart(&m_rectangleShape);
    m_rectangleShape.setOutlineColor(sf::Color::White);
    m_rectangleShape.setOutlineThickness(2.f);
    m_rectangleShape.setOrigin(0.f, 1.f);
}

//-------------------//
//----- Routine -----//

void CustomLine::onSizeChanges()
{
    m_rectangleShape.setSize(size());
    setScale(scale());
}

//--------------------//
//----- Wrappers -----//

void CustomLine::setLimits(const sf::Vector2f& start, const sf::Vector2f& end)
{
    auto direction = end - start;
    auto length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    m_rectangleShape.setSize({length, 1.f});

    float alpha = 180.f * std::atan2(direction.y, direction.x) / M_PI;
    m_rectangleShape.setRotation(alpha);
    m_rectangleShape.setPosition(start);
}
