#include "scene/posteffects/floomzig.hpp"

#include "context/context.hpp"
#include "tools/vector.hpp"
#include "tools/tools.hpp"

#include <SFML/Graphics/Sprite.hpp>

using namespace scene;

Floomzig::Floomzig()
{
    m_sobelShader = &context::context.shaders.get("core/posteffects/floomzig/sobel");
    m_addShader = &context::context.shaders.get("core/posteffects/floomzig/add");

    const auto& floomTexture = context::context.textures.get("core/posteffects/floomzig/floom");
    m_sobelShader->setParameter("floom", floomTexture);
    m_sobelShader->setParameter("floomSize", sf::v2f(floomTexture.getSize()));
}

//-------------------//
//----- Routine -----//

void Floomzig::apply(sf::RenderTarget& out, const sf::RenderTexture& in)
{
    prepareTextures(in.getSize());

    // Note: we need the temporary target because out could be our in
    sobel(m_sobelTexture, in);
    m_sobelTexture.display();

    add(out, in, m_sobelTexture);
}

//------------------//
//----- Effect -----//

void Floomzig::prepareTextures(const sf::Vector2u& size)
{
    returnif (m_sobelTexture.getSize() == size);

    m_sobelTexture.create(size.x, size.y);
    m_sobelTexture.setSmooth(true);
}

void Floomzig::sobel(sf::RenderTarget& out, const sf::RenderTexture& in)
{
    m_sobelShader->setParameter("source", in.getTexture());
    m_sobelShader->setParameter("sourceSize", sf::v2f(in.getSize()));

    sf::Vector2f floomOffset;
    floomOffset.x = static_cast<float>(15.f * sin(rand()));
    floomOffset.y = static_cast<float>(15.f * cos(rand()));
    m_sobelShader->setParameter("floomOffset", floomOffset);

    shaderize(out, *m_sobelShader);
}

void Floomzig::add(sf::RenderTarget& out, const sf::RenderTexture& in1, const sf::RenderTexture& in2)
{
    m_addShader->setParameter("source1", in1.getTexture());
    m_addShader->setParameter("source2", in2.getTexture());
    shaderize(out, *m_addShader);
}
