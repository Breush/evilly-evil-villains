#include "sfe/arrow.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

using namespace sfe;

//----- Arrow -----//

Arrow::Arrow()
    : m_length(0.f)
    , m_vertices(sf::TrianglesFan, 4)
{
    m_vertices[0].color = sf::Color::White;
    m_vertices[1].color = sf::Color::White;
    m_vertices[2].color = sf::Color::Transparent;
    m_vertices[3].color = sf::Color::White;
}

void Arrow::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    // Transform from sf::Tranformable
    states.transform *= getTransform();

    // Drawing
    target.draw(m_vertices, states);
}

//----- LArrow -----//

void LArrow::update()
{
    m_vertices[0].position = {length()/2.f, length()/2.f};
    m_vertices[1].position = {length(),     length()};
    m_vertices[2].position = {0.f,          length()/2.f};
    m_vertices[3].position = {length(),     0.f};
}

//----- RArrow -----//

void RArrow::update()
{
    m_vertices[0].position = {length()/2.f, length()/2.f};
    m_vertices[1].position = {0.f,          length()};
    m_vertices[2].position = {length(),     length()/2.f};
    m_vertices[3].position = {0.f,          0.f};
}

