#include "scene/posteffects/posteffect.hpp"

#include "tools/vector.hpp"

using namespace scene;

PostEffect::PostEffect()
    : m_vertices(sf::TrianglesStrip, 4u)
{
    m_vertices[0u].texCoords = {0.f, 1.f};
    m_vertices[1u].texCoords = {1.f, 1.f};
    m_vertices[2u].texCoords = {0.f, 0.f};
    m_vertices[3u].texCoords = {1.f, 0.f};
}

//-----------------//
//----- Tools -----//

void PostEffect::shaderize(sf::RenderTarget& out, const sf::Shader& shader, sf::BlendMode blendMode)
{
    auto size = sf::v2f(out.getSize());
    m_vertices[0u].position = {0.f, 0.f};
    m_vertices[1u].position = {size.x, 0.f};
    m_vertices[2u].position = {0.f, size.y};
    m_vertices[3u].position = size;

    m_states.shader = &shader;
    m_states.blendMode = blendMode;

    out.draw(m_vertices, m_states);
}
