#include "sfe/line.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

using namespace sfe;

//----- Line -----//

Line::Line()
    : m_length(0.f)
    , m_shade(0.15)
    , m_vertices(sf::LinesStrip, 4)
{
    m_vertices[0].color = sf::Color::Transparent;
    m_vertices[1].color = sf::Color::White;
    m_vertices[2].color = sf::Color::White;
    m_vertices[3].color = sf::Color::Transparent;
}

void Line::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    // Transform from sf::Tranformable
    states.transform *= getTransform();

    // Drawing
    target.draw(m_vertices, states);
}

//----- HLine -----//

void HLine::update()
{
    m_vertices[0].position = {0.f, 0.f};
    m_vertices[1].position = {shade() * length(), 0.f};
    m_vertices[2].position = {(1.f - shade()) * length(), 0.f};
    m_vertices[3].position = {length(), 0.f};
}

//----- VLine -----//

void VLine::update()
{
    m_vertices[0].position = {0.f, 0.f};
    m_vertices[1].position = {0.f, shade() * length()};
    m_vertices[2].position = {0.f, (1.f - shade()) * length()};
    m_vertices[3].position = {0.f, length()};
}
