#include "scene/scene.hpp"

#include "context/context.hpp"
#include "scene/layer.hpp"
#include "tools/tools.hpp"
#include "tools/vector.hpp"
#include "tools/platform-fixes.hpp" // reverse

#include <SFML/Graphics/RenderTarget.hpp>

using namespace scene;

Scene::Scene(Graph* graph)
    : m_minZoom(0.4f)
    , m_maxZoom(1.f)
    , m_graph(graph)
    , m_relativeCenter(0.5f, 0.5f)
    , m_moveFriction(10.f)
{
}

//-------------------//
//----- Routine -----//

void Scene::update(const sf::Time& dt, const float factor)
{
    for (auto& layer : m_layers)
        layer->update(dt, factor);

    // Smooth moving
    if (std::abs(m_moveVelocity.x) >= 0.01f || std::abs(m_moveVelocity.y) >= 0.01f) {
        sf::Vector2f offset;

        offset += m_moveVelocity;
        m_moveVelocity -= dt.asSeconds() * m_moveFriction * m_moveVelocity;

        m_refView.move(offset);
        adaptViewPosition();
    }
}

void Scene::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    // Layers
    for (auto& layer : m_layers)
        target.draw(*layer, states);
}

void Scene::refreshWindow(const config::WindowInfo& cWindow)
{
    // Viewport
    if (!m_ownViewport)
        m_refView.setViewport(cWindow.viewport);

    // Layers
    for (auto& layer : m_layers)
        layer->refreshWindow(cWindow);
}

void Scene::refreshNUI(const config::NUIGuides& cNUI)
{
    // Layers
    for (auto& layer : m_layers)
        layer->refreshNUI(cNUI);
}

//----------------------------//
//----- Layer management -----//

Layer& Scene::addLayer(const std::string& key, float depth)
{
    // Create a layer and set default parameters
    auto layer = std::make_shared<Layer>();
    layer->init(m_graph);

    layer->setManipulable(true);
    layer->setSize(m_size);
    layer->setDepth(depth);
    layer->setViewSize(m_refView.getSize() * depth);
    if (m_ownViewport) layer->setViewport(m_viewport);

    // Add to containers keep track
    m_layers.emplace_back(layer);
    m_layersMap.emplace(key, layer);

    // Sort the container used to draw
    m_layers.sort([](std::shared_ptr<Layer> a, std::shared_ptr<Layer> b) { return a->depth() > b->depth(); });

    // If the new layer is the deeper,
    // the max view size need to be updated
    if (depth > m_maxDepth) {
        m_maxDepth = depth;
        updateReferenceMinMax();
    }
    // If not, still set correct position
    else {
        updateLayersPosition();
    }

    return *layer;
}

//-----------------------------//
//----- View manipulation -----//

void Scene::setViewport(const sf::FloatRect& viewport)
{
    m_ownViewport = true;
    m_viewport = viewport;
    m_refView.setViewport(m_viewport);

    for (auto& layer : m_layers)
        layer->setViewport(m_viewport);

    updateReferenceMinMax();
}

void Scene::centerRelative(const sf::Vector2f& relCoords)
{
    m_refView.move(m_refView.getSize() / 2.f + relCoords * (m_size - m_refView.getSize()));
    adaptViewPosition();
}

void Scene::zoom(const sf::Vector2i& mousePos, float zoomFactor)
{
    const auto& window = context::context.window;
    m_relZoom *= zoomFactor;

    // Apply new zoom
    auto initPos = window.mapPixelToCoords(mousePos, m_refView);
    m_refView.zoom(zoomFactor);
    adaptViewZoom();

    // Move to adjust the point below the mouse
    auto finalPos = window.mapPixelToCoords(mousePos, m_refView);
    m_refView.move(initPos - finalPos);
    adaptViewPosition();
}

void Scene::startGrabbing(const sf::Vector2i& mousePos)
{
    const auto& window = context::context.window;
    auto position = window.mapPixelToCoords(mousePos, m_refView);
    m_grabbingMousePosition = mousePos;
    m_grabbingPosition = position;
}

void Scene::moveGrabbing(const sf::Vector2i& mousePos)
{
    const auto& window = context::context.window;
    auto position = window.mapPixelToCoords(mousePos, m_refView);
    m_refView.move(m_grabbingPosition - position);
    adaptViewPosition();
}

void Scene::moveGrabbing(const sf::Vector2i& mousePos, const sf::Time& dt)
{
    returnif (m_grabbingMousePosition == mousePos);
    m_refView.move(3.f * sf::v2f(mousePos - m_grabbingMousePosition) * dt.asSeconds());
    adaptViewPosition();
}

void Scene::moveView(const sf::Vector2f& offset)
{
    // This coefficient is an experimental value.
    m_moveVelocity += 0.5f * offset;
}

void Scene::setViewCenter(const sf::Vector2f& position)
{
    m_refView.setCenter(position);
    adaptViewPosition();
}

Entity* Scene::entityFromPosition(const sf::Vector2i& mousePos, sf::Vector2f& viewPos)
{
    for (const auto& layer : std::reverse(m_layers)) {
        Entity* entity = layer->entityFromPosition(mousePos, viewPos);
        returnif (entity != nullptr) entity;
    }

    return nullptr;
}

//------------------//
//----- Limits -----//

void Scene::adaptViewPosition()
{
    const auto& viewHalfSize = m_refView.getSize() / 2.f;
    const auto& viewCenter = m_refView.getCenter();

    const auto offLeft = (viewCenter.x - viewHalfSize.x);
    const auto offRight = (viewCenter.x + viewHalfSize.x) - m_size.x;

    if (offLeft < 0.f) m_refView.move(-offLeft, 0.f);
    else if (offRight > 0.f) m_refView.move(-offRight, 0.f);

    const auto offTop = (viewCenter.y - viewHalfSize.y);
    const auto offBottom = (viewCenter.y + viewHalfSize.y) - m_size.y;

    if (offTop < 0.f) m_refView.move(0.f, -offTop);
    else if (offBottom > 0.f) m_refView.move(0.f, -offBottom);

    updateLayersPosition();
}

void Scene::adaptViewZoom()
{
    if (m_relZoom <= m_minZoom) {
        m_relZoom = m_minZoom;
        m_refView.setSize(m_minSize);
    }
    else if (m_relZoom >= m_maxZoom) {
        m_relZoom = m_maxZoom;
        m_refView.setSize(m_maxSize);
    }

    updateLayersZoom();
}

//-------------------//
//----- Changes -----//

void Scene::updateReferenceMinMax()
{
    const auto& viewport = (m_ownViewport)? m_viewport : context::context.windowInfo.viewport;
    const auto& screenSize = context::context.windowInfo.screenSize;
    sf::Vector2f viewportSize{viewport.width * screenSize.x, viewport.height * screenSize.y};

    auto viewRatio = viewportSize.x / viewportSize.y;
    auto layerRatio = m_size.x / m_size.y;

    // The view reference size is the maximum valid size that
    // does not show anything beyond what displayRect tells.
    sf::Vector2f viewRefSize;
    if (viewRatio < layerRatio) viewRefSize = viewportSize * m_size.y / viewportSize.y;
    else viewRefSize = viewportSize * m_size.x / viewportSize.x;

    m_minSize = m_minZoom * viewRefSize;
    m_maxSize = m_maxZoom * viewRefSize;

    m_refView.setSize(m_maxSize);
    m_refView.setCenter(m_size / 2.f);
    m_relZoom = m_maxZoom;

    updateLayersZoom();
}

void Scene::updateLayersZoom()
{
    for (auto& layer : m_layers)
        layer->setViewSize(m_refView.getSize() * layer->depth());

    // As relative center might have changed
    updateLayersPosition();
}

void Scene::updateLayersPosition()
{
    auto relativeCenter = (m_refView.getCenter() - m_refView.getSize() / 2.f) / (m_size - m_refView.getSize());

    // Avoid division by zero
    // TODO Even with this previous value trick, there is still some shaking when zooming in from max.
    if (m_size.x == m_refView.getSize().x) relativeCenter.x = m_relativeCenter.x;
    if (m_size.y == m_refView.getSize().y) relativeCenter.y = m_relativeCenter.y;

    m_relativeCenter = relativeCenter;

    for (auto& layer : m_layers)
        layer->setRelativeCenter(relativeCenter);
}
