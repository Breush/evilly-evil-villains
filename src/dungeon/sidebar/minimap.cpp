#include "dungeon/sidebar/minimap.hpp"

#include "context/context.hpp"
#include "scene/graph.hpp"
#include "scene/layer.hpp"
#include "tools/vector.hpp"
#include "tools/tools.hpp"

using namespace dungeon;

Minimap::Minimap(const scene::Graph& graph)
    : m_nuiView(graph.nuiLayer().view())
{
    // Background
    addPart(&m_background);
    m_background.setOutlineThickness(1.f);
    m_background.setOutlineColor(sf::Color::White);
    m_background.setFillColor({255u, 255u, 255u, 150u});

    // Layer view indicator
    m_layerViewIndicator.setFillColor({255u, 255u, 255u, 100u});
    m_layerViewIndicator.setOutlineColor(sf::Color::White);
    m_layerViewIndicator.setOutlineThickness(1.f);
}

void Minimap::init()
{
}

//-------------------//
//----- Routine -----//

void Minimap::onTransformChanges()
{
    refreshViewport();
}

void Minimap::drawInternal(sf::RenderTarget& target, sf::RenderStates states) const
{
    // Draw the minimap
    if (m_layer != nullptr) {
        auto previousView = target.getView();
        target.setView(m_view);
        target.draw(m_layer->root());
        target.setView(previousView);
    }

    // Layer indicator overlay
    target.draw(m_layerViewIndicator, states);
}

void Minimap::onSizeChanges()
{
    m_background.setSize(size());
    refreshViewport();
}

void Minimap::refreshWindow(const config::WindowInfo& cWindow)
{
    baseClass::refreshWindow(cWindow);

    refreshViewport();
}

void Minimap::refreshNUI(const config::NUIGuides& cNUI)
{
    baseClass::refreshNUI(cNUI);
}

//------------------//
//----- Events -----//

void Minimap::handleGlobalMouseMoved(const sf::Vector2f& nuiPos)
{
    returnif (!m_grabbing);
    doAction(nuiPos);
}

void Minimap::handleGlobalMouseButtonReleased(const sf::Mouse::Button button, const sf::Vector2f& nuiPos)
{
    returnif (!m_grabbing);
    returnif (button != sf::Mouse::Left);
    m_grabbing = false;
}

bool Minimap::handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos)
{
    returnif (button != sf::Mouse::Left) false;
    m_grabbing = true;
    doAction(nuiPos);
    return true;
}

//------------------//
//----- Action -----//

void Minimap::doAction(const sf::Vector2f& nuiPos)
{
    returnif (m_layer == nullptr);
    returnif (m_callbackAction == nullptr);

    const auto& window = context::context.window;
    auto pixel = window.mapCoordsToPixel(nuiPos, m_nuiView);
    auto viewCoordsClicked = window.mapPixelToCoords(pixel, m_view);
    m_callbackAction(viewCoordsClicked);
}

//-----------------//
//----- Layer -----//

void Minimap::setLayer(scene::Layer& layer)
{
    m_layer = &layer;
    returnif (m_layer == nullptr);

    // Refresh
    refreshSize();

    // Callback
    m_layer->callOnSizeChanges([this] { refreshSize(); });
    m_layer->callOnViewChanges([this] { refreshViewIndicator(); });
}

//-----------------------------------//
//----- Internal changes update -----//

void Minimap::refreshSize()
{
    returnif (m_layer == nullptr);

    setSize(m_layer->size() / 15.f);
    m_view.setSize(m_layer->size());
    m_view.setCenter(m_layer->size() / 2.f);

    refreshViewIndicator();
}

void Minimap::refreshViewport()
{
    const auto& window = context::context.window;
    const auto& screenSize = context::context.windowInfo.screenSize;
    const auto& referenceView = context::context.windowInfo.referenceView;
    sf::FloatRect rect{getPosition().x, getPosition().y, size().x, size().y};
    rect = tools::mapRectCoordsToPixel(window, rect, &referenceView);
    m_view.setViewport(rect / screenSize);
}

void Minimap::refreshViewIndicator()
{
    returnif (m_layer == nullptr);

    auto relativeLayerViewCenter = m_layer->view().getCenter() / m_layer->size();
    auto relativeLayerViewSize = m_layer->view().getSize() / m_layer->size();
    m_layerViewIndicator.setSize(size() * relativeLayerViewSize);
    m_layerViewIndicator.setPosition(size() * (relativeLayerViewCenter - relativeLayerViewSize / 2.f));
}
