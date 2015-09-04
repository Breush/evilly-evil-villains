#include "sfe/arrow.hpp"

#include "tools/tools.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

using namespace sfe;

//-----------------//
//----- Arrow -----//

Arrow::Arrow()
    : m_length(0.f)
    , m_vertices(sf::TrianglesFan, 4)
{
    m_vertices[0].color = {255u, 255u, 255u, 120u};
    m_vertices[1].color = sf::Color::White;
    m_vertices[2].color = sf::Color::White;
    m_vertices[3].color = sf::Color::White;
}

void Arrow::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    returnif (m_length < 1.f);

    // Transform from sf::Tranformable
    states.transform *= getTransform();

    // Drawing
    target.draw(m_sprite, states);
}

void Arrow::setLength(float inLength)
{
    returnif (m_length == inLength || inLength < 1.f);
    m_length = inLength;
    updateFromLength();
}

void Arrow::refreshRenderTexture()
{
    m_renderTexture.create(m_length + 1u, m_length + 1u);
    m_renderTexture.setSmooth(true);

    m_renderTexture.clear(sf::Color::Transparent);
    m_renderTexture.draw(m_vertices);
    m_renderTexture.display();

    m_sprite.setTexture(m_renderTexture.getTexture());
}

//------------------//
//----- LArrow -----//

void LArrow::updateFromLength()
{
    m_vertices[0].position = {length()/2.f, length()/2.f};
    m_vertices[1].position = {length(),     length()};
    m_vertices[2].position = {0.f,          length()/2.f};
    m_vertices[3].position = {length(),     0.f};

    baseClass::refreshRenderTexture();
}

//------------------//
//----- RArrow -----//

void RArrow::updateFromLength()
{
    m_vertices[0].position = {length()/2.f, length()/2.f};
    m_vertices[1].position = {0.f,          length()};
    m_vertices[2].position = {length(),     length()/2.f};
    m_vertices[3].position = {0.f,          0.f};

    baseClass::refreshRenderTexture();
}
