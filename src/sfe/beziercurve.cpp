#include "sfe/beziercurve.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

using namespace sfe;

BezierCurve::BezierCurve()
    : m_vertices(sf::LinesStrip, 25u)
{
    setColor(sf::Color::White);
}

void BezierCurve::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    target.draw(m_vertices, states);
}

//-------------------//
//----- Control -----//

void BezierCurve::setColor(const sf::Color &color)
{
    for (uint i = 0u; i < m_vertices.getVertexCount(); ++i)
        m_vertices[i].color = color;
}

void BezierCurve::set(const sf::Vector2f& s1, const sf::Vector2f& s2, const sf::Vector2f& c1, const sf::Vector2f& c2)
{
    // First and last points are fixed
    m_vertices[0u].position = s1;
    m_vertices[m_vertices.getVertexCount() - 1u].position = s2;

    // Fixed coefficients
    const float q = 1.f / (m_vertices.getVertexCount() - 1u);
    const auto w3 = s2 + 3.f * (c1 - c2) - s1;
    const auto w2 = 3.f * (s1 - 2.f * c1 + c2);
    const auto w1 = 3.f * (c1 - s1);
    const auto w0 = s1;

    // Computed intermediate points
    float p = q;
    for (uint i = 1u; i < m_vertices.getVertexCount() - 1u; ++i) {
        m_vertices[i].position = w0;
        m_vertices[i].position += p * w1;
        m_vertices[i].position += p * p * w2;
        m_vertices[i].position += p * p * p * w3;
        p += q;
    }
}
