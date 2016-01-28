#include "scene/layer.hpp"

#include "context/context.hpp"
#include "tools/tools.hpp"
#include "tools/vector.hpp"

#include <SFML/Graphics/View.hpp>

using namespace scene;

LayerRoot::LayerRoot()
{
    setDetectable(false);
    setFocusable(false);
    setTransparent(true);
}

Layer::Layer()
{
    // Lighting
    m_lightRenderStates.blendMode = sf::BlendMultiply;

    m_penumbraTexture = &context::context.textures.get("core/ltbl/penumbra");
    m_lightOverShapeShader = &context::context.shaders.get("core/ltbl/lightOverShape");
    m_unshadowShader = &context::context.shaders.get("core/ltbl/unshadow");
    m_normalsShader = &context::context.shaders.get("core/ltbl/normals");
}

void Layer::init(Graph* graph)
{
    m_root.setGraph(graph);
    m_root.setLayer(this);
}

//-------------------//
//----- Routine -----//

void Layer::update(const sf::Time& dt, const float factor)
{
    m_root.update(dt, factor);
}

void Layer::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    // No lights? Easy drawing.
    if (!m_lightsOn) {
        target.setView(m_view);
        target.draw(m_root, states);
        return;
    }

    // Normals prologue
    m_lightSystem.normalsTargetClear();
    m_lightSystem.normalsTargetSetView(m_internView);

    // We keep an intermediate RenderTarget so that the lighting can affect only this layer
    m_tmpTarget.clear(sf::Color::Transparent);
    m_tmpTarget.setView(m_internView);
    m_tmpTarget.draw(m_root, states);

    // Normals epilogue
    m_lightSystem.normalsTargetDisplay();

    // We are rendering within the effective view
    m_lightSystem.render(m_internView, *m_unshadowShader, *m_lightOverShapeShader, *m_normalsShader);

    // But we show the lighting sprite in {0.f, 0.f}
    sf::Sprite lightSprite(m_lightSystem.getLightingTexture());
    m_tmpTarget.setView(m_tmpTarget.getDefaultView());
    m_tmpTarget.draw(lightSprite, m_lightRenderStates);
    m_tmpTarget.display();

    // Draw to the final target
    sf::Sprite screenSprite(m_tmpTarget.getTexture());
    target.setView(m_basicView);
    target.draw(screenSprite);
}

void Layer::refreshSize()
{
    // Set the root entity to the size of the layer.
    // This is used for relative positionning of entities.
    m_root.setSize(m_size);
    refreshManipulability();
    refreshLightSystem();

    if (m_onSizeChangesCallback != nullptr)
        m_onSizeChangesCallback();
}

void Layer::refreshWindow(const config::WindowInfo& cWindow)
{
    if (!m_ownViewport)
        m_view.setViewport(cWindow.viewport);

    // Refresh the basic view, using the screenSize
    refreshBasicView();

    // Recursively update the whole layer.
    m_root.refreshWindow(cWindow);
}

void Layer::refreshNUI(const config::NUIGuides& cNUI)
{
    // Recursively update the whole layer.
    m_root.refreshNUI(cNUI);
}

//--------------------//
//----- Lighting -----//

void Layer::turnLights(bool on)
{
    m_lightsOn = on;
    refreshLightSystem();
}

//-------------------//
//----- Getters -----//

sf::FloatRect Layer::visibleRect() const
{
    sf::FloatRect rect{0.f, 0.f, m_size.x, m_size.y};
    const auto& viewSize = m_view.getSize();

    if (m_size.x < viewSize.x) {
        rect.left = m_size.x - viewSize.x;
        rect.width = 2 * viewSize.x - m_size.x;
    }

    if (m_size.y < viewSize.y) {
        rect.top = m_size.y - viewSize.y;
        rect.height = 2 * viewSize.y - m_size.y;
    }

    return rect;
}

void Layer::fitToVisibleRect(scene::Entity& entity)
{
    auto rect = visibleRect();
    entity.setSize({rect.width, rect.height});
    entity.setLocalPosition({rect.left, rect.top});
}

//-----------------------------//
//----- View manipulation -----//

void Layer::setViewport(const sf::FloatRect& viewport)
{
    m_ownViewport = true;
    m_view.setViewport(viewport);
    refreshBasicView();
}

void Layer::setRelativeCenter(const sf::Vector2f& relativeCenter)
{
    m_view.setCenter(m_view.getSize() / 2.f + relativeCenter * (m_size - m_view.getSize()));
    m_internView.setCenter(m_view.getCenter());

    if (m_onViewChangesCallback != nullptr)
        m_onViewChangesCallback();
}

void Layer::setViewSize(const sf::Vector2f& viewSize)
{
    m_view.setSize(viewSize);
    m_internView.setSize(m_view.getSize());

    if (m_onViewChangesCallback != nullptr)
        m_onViewChangesCallback();
}

Entity* Layer::entityFromPosition(const sf::Vector2i& mousePos, sf::Vector2f& viewPos)
{
    const auto& window = context::context.window;
    viewPos = window.mapPixelToCoords(mousePos, m_view);
    return m_root.firstOver(viewPos);
}

void Layer::refreshManipulability()
{
    if (!m_manipulable) {
        m_view.setSize(m_size);
        m_view.setCenter(m_size / 2.f);

        m_internView.setSize(m_view.getSize());
        m_internView.setCenter(m_view.getCenter());
    }
}

//---------------//
//----- ICU -----//

void Layer::refreshBasicView()
{
    const auto& viewport = m_view.getViewport();
    const auto& resolution = context::context.display.window.resolution;

    m_basicView.setViewport(viewport);
    m_basicView.setSize({viewport.width * resolution.x, viewport.height * resolution.y});
    m_basicView.setCenter(0.5f * m_basicView.getSize());

    // Some light system's parts are based on basic view informations
    refreshLightSystem();
}

void Layer::refreshLightSystem()
{
    returnif (!m_lightsOn);

    m_tmpTarget.create(m_basicView.getSize().x, m_basicView.getSize().y);
    m_tmpTarget.setSmooth(true);
    m_lightSystem.create({0.f, 0.f, m_size.x, m_size.y}, sf::v2u(m_basicView.getSize()), *m_penumbraTexture, *m_unshadowShader, *m_lightOverShapeShader, *m_normalsShader);
    // m_lightSystem.normalsEnabled(true);

    // FIXME The following is a debug thingy
    if (m_lightDebugFirstTime) {
        m_lightDebugFirstTime = false;

        const auto& lightPointTexture = context::context.textures.get("core/ltbl/lightPoint");
        auto light = std::make_shared<ltbl::LightPointEmission>();
        light->_emissionSprite.setOrigin({lightPointTexture.getSize().x * 0.5f, lightPointTexture.getSize().y * 0.5f});
        light->_emissionSprite.setTexture(lightPointTexture);
        light->_emissionSprite.setScale({40.0f, 40.0f});
        light->_emissionSprite.setColor({255u, 230u, 200u});
        light->_emissionSprite.setPosition(0.5f * m_size.x, 0.75f * m_size.y);
        m_lightSystem.addLight(light);
    }
}
