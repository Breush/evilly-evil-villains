#include "scene/posteffects/motionblur.hpp"

#include "context/context.hpp"
#include "tools/vector.hpp"
#include "tools/tools.hpp"

#include <SFML/Graphics/Sprite.hpp>

using namespace scene;

MotionBlur::MotionBlur()
{
    m_mixShader = &context::context.shaders.get("core/posteffects/motionblur/mix");
}

//-------------------//
//----- Routine -----//

void MotionBlur::apply(sf::RenderTarget& out, const sf::RenderTexture& in)
{
    prepareTextures(in.getSize());

    mix(m_drawingTexture, m_memoryTexture, in);
    m_drawingTexture.display();

    // Save as the current frame
    sf::Sprite insprite(in.getTexture());
    m_memoryTexture.draw(insprite, sf::BlendNone);
    m_memoryTexture.display();

    // TODO Bigger motion blur?

    // Draw to the output target
    sf::Sprite sprite(m_drawingTexture.getTexture());
    out.draw(sprite, sf::BlendNone);
}

//------------------//
//----- Effect -----//

void MotionBlur::prepareTextures(const sf::Vector2u& size)
{
    returnif (m_memoryTexture.getSize() == size);

    m_memoryTexture.create(size.x, size.y);
    m_memoryTexture.setSmooth(true);

    m_drawingTexture.create(size.x, size.y);
    m_drawingTexture.setSmooth(true);
}

void MotionBlur::mix(sf::RenderTarget& out, const sf::RenderTexture& in1, const sf::RenderTexture& in2)
{
    m_mixShader->setParameter("source1", in1.getTexture());
    m_mixShader->setParameter("source2", in2.getTexture());
    shaderize(out, *m_mixShader);
}
