#include "sfe/rectanglegradient.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

using namespace sfe;

RectangleGradient::RectangleGradient()
    : m_vertices(sf::TrianglesStrip, 4u)
{
    for (auto i = 0u; i < m_vertices.getVertexCount(); ++i)
        m_vertices[i].color = sf::Color::Transparent;
}

//-------------------//
//----- Routine -----//

void RectangleGradient::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    target.draw(m_vertices, states);
}

//-------------------//
//----- Control -----//

void RectangleGradient::setSize(const sf::Vector2f& size)
{
    m_vertices[PointPosition::TOP_LEFT].position = {0.f, 0.f};
    m_vertices[PointPosition::TOP_RIGHT].position = {size.x, 0.f};
    m_vertices[PointPosition::BOTTOM_LEFT].position = {0.f, size.y};
    m_vertices[PointPosition::BOTTOM_RIGHT].position = {size.x, size.y};
}

void RectangleGradient::setPointColor(const PointPosition pointPosition, const sf::Color& color)
{
    m_vertices[pointPosition].color = color;
}

void RectangleGradient::setPointsColors(const sf::Color& topColor, const sf::Color& bottomColor)
{
    m_vertices[PointPosition::TOP_LEFT].color = topColor;
    m_vertices[PointPosition::TOP_RIGHT].color = topColor;
    m_vertices[PointPosition::BOTTOM_LEFT].color = bottomColor;
    m_vertices[PointPosition::BOTTOM_RIGHT].color = bottomColor;
}
