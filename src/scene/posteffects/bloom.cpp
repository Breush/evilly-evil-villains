#include "scene/posteffects/bloom.hpp"

#include "context/context.hpp"
#include "tools/vector.hpp"
#include "tools/tools.hpp"

using namespace scene;

Bloom::Bloom()
{
    m_brightShader = &context::context.shaders.get("core/posteffects/bloom/bright");
    m_downSampleShader = &context::context.shaders.get("core/posteffects/bloom/downSample");
    m_gaussianBlurShader = &context::context.shaders.get("core/posteffects/bloom/gaussianBlur");
    m_addShader = &context::context.shaders.get("core/posteffects/bloom/add");
}

//-------------------//
//----- Routine -----//

void Bloom::apply(sf::RenderTarget& out, const sf::RenderTexture& in)
{
    prepareTextures(in.getSize());

    filterBright(m_brightTexture, in);

    downsample(m_firstPassTextures[0], m_brightTexture);
    blurMultipass(m_firstPassTextures);

    downsample(m_secondPassTextures[0], m_firstPassTextures[0]);
    blurMultipass(m_secondPassTextures);

    add(m_firstPassTextures[1], m_firstPassTextures[0], m_secondPassTextures[0]);
    m_firstPassTextures[1].display();

    add(out, in, m_firstPassTextures[1]);
}

//------------------//
//----- Effect -----//

void Bloom::prepareTextures(const sf::Vector2u& size)
{
    returnif (m_brightTexture.getSize() == size);

    m_brightTexture.create(size.x, size.y);
    m_brightTexture.setSmooth(true);

    m_firstPassTextures[0u].create(size.x / 2u, size.y / 2u);
    m_firstPassTextures[0u].setSmooth(true);
    m_firstPassTextures[1u].create(size.x / 2u, size.y / 2u);
    m_firstPassTextures[1u].setSmooth(true);

    m_secondPassTextures[0u].create(size.x / 4u, size.y / 4u);
    m_secondPassTextures[0u].setSmooth(true);
    m_secondPassTextures[1u].create(size.x / 4u, size.y / 4u);
    m_secondPassTextures[1u].setSmooth(true);
}

void Bloom::filterBright(sf::RenderTexture& out, const sf::RenderTexture& in)
{
    m_brightShader->setParameter("source", in.getTexture());
    shaderize(out, *m_brightShader);
    out.display();
}

void Bloom::blurMultipass(RenderTextureArray& textures)
{
    auto textureSize = textures[0].getSize();

    blur(textures[1u], textures[0u], {0.f, 1.f / textureSize.y});
    blur(textures[0u], textures[1u], {1.f / textureSize.x, 0.f});
    blur(textures[1u], textures[0u], {0.f, 1.f / textureSize.y});
    blur(textures[0u], textures[1u], {1.f / textureSize.x, 0.f});
}

void Bloom::blur(sf::RenderTexture& out, const sf::RenderTexture& in, const sf::Vector2f& offsetFactor)
{
    m_gaussianBlurShader->setParameter("source", in.getTexture());
    m_gaussianBlurShader->setParameter("offsetFactor", offsetFactor);
    shaderize(out, *m_gaussianBlurShader);
    out.display();
}

void Bloom::downsample(sf::RenderTexture& out, const sf::RenderTexture& in)
{
    m_downSampleShader->setParameter("source", in.getTexture());
    m_downSampleShader->setParameter("sourceSize", sf::v2f(in.getSize()));
    shaderize(out, *m_downSampleShader);
    out.display();
}

void Bloom::add(sf::RenderTarget& out, const sf::RenderTexture& in1, const sf::RenderTexture& in2)
{
    m_addShader->setParameter("source1", in1.getTexture());
    m_addShader->setParameter("source2", in2.getTexture());
    shaderize(out, *m_addShader);
}
