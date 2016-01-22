#include "scene/components/lightemitter.hpp"

#include "core/application.hpp"
#include "scene/layer.hpp"
#include "tools/tools.hpp"

using namespace scene;

LightEmitter::LightEmitter(Entity& entity)
    : baseClass(entity)
{
}

LightEmitter::~LightEmitter()
{
    clear();
}

//---------------------//
//----- Callbacks -----//

void LightEmitter::onTransformChanged()
{
    const auto& entityTransform = m_entity.getTransform();
    for (const auto& light : m_lights)
        light.point->_emissionSprite.setPosition(entityTransform.transformPoint(light.position));
}

void LightEmitter::onLayerChanged(Layer* layer)
{
    // Detach from previous
    if (m_lightSystem != nullptr)
        for (const auto& light : m_lights)
            m_lightSystem->removeLight(light.point);

    // Test new viability
    if (layer == nullptr) {
        m_lightSystem = nullptr;
        return;
    }

    m_lightSystem = &layer->lightSystem();

    // Add to new
    for (const auto& light : m_lights)
        m_lightSystem->addLight(light.point);
}

//----------------------------//
//----- Lighting control -----//

void LightEmitter::clear()
{
    if (m_lightSystem != nullptr)
        for (const auto& light : m_lights)
            m_lightSystem->removeLight(light.point);
    m_lights.clear();
}

uint32 LightEmitter::addPoint(const sf::Vector2f& position, float lightSize)
{
    const auto& lightPointTexture = Application::context().textures.get("core/ltbl/lightPoint");
    auto lightPointTextureSize = sf::v2f(lightPointTexture.getSize());
    float scale = lightSize / lightPointTextureSize.x;

    Light light;
    light.position = position;
    light.point = std::make_shared<ltbl::LightPointEmission>();
    light.point->_emissionSprite.setOrigin(0.5f * lightPointTextureSize);
    light.point->_emissionSprite.setTexture(lightPointTexture);
    light.point->_emissionSprite.setScale(scale, scale);
    light.point->_emissionSprite.setColor(sf::Color::White);
    light.point->_emissionSprite.setPosition(m_entity.getTransform().transformPoint(position));

    if (m_lightSystem != nullptr)
        m_lightSystem->addLight(light.point);

    m_lights.push_back(std::move(light));
    return m_lights.size() - 1u;
}

void LightEmitter::setColor(uint32 lightID, const sf::Color& color)
{
    returnif (lightID >= m_lights.size());
    m_lights[lightID].point->_emissionSprite.setColor(color);
}
