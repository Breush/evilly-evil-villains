#include "scene/layer.hpp"

#include "core/application.hpp"
#include "tools/tools.hpp"
#include "tools/vector.hpp"

#include <SFML/Graphics/View.hpp>

using namespace scene;

Layer::Layer(Graph* graph)
{
    m_root.setGraph(graph);
    m_root.setDetectable(false);
    m_root.setFocusable(false);
    m_root.setTransparent(true);
}

//-------------------//
//----- Routine -----//

void Layer::update(const sf::Time& dt)
{
    m_root.update(dt);
}

void Layer::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.setView(m_view);
    target.draw(m_root, states);
}

void Layer::refreshSize()
{
    // Set the root entity to the size of the layer.
    // This is used for relative positionning of entities.
    m_root.setSize(m_size);
    refreshManipulability();

    if (m_onSizeChangesCallback != nullptr)
        m_onSizeChangesCallback();
}

void Layer::refreshWindow(const config::WindowInfo& cWindow)
{
    if (!m_ownViewport)
        m_view.setViewport(cWindow.viewport);

    // Recursively update the whole layer.
    m_root.refreshWindow(cWindow);
}

void Layer::refreshNUI(const config::NUIGuides& cNUI)
{
    // Recursively update the whole layer.
    m_root.refreshNUI(cNUI);
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
}

void Layer::setRelativeCenter(const sf::Vector2f& relativeCenter)
{
    m_view.setCenter(m_view.getSize() / 2.f + relativeCenter * (m_size - m_view.getSize()));

    if (m_onViewChangesCallback != nullptr)
        m_onViewChangesCallback();
}

void Layer::setViewSize(const sf::Vector2f& viewSize)
{
    m_view.setSize(viewSize);

    if (m_onViewChangesCallback != nullptr)
        m_onViewChangesCallback();
}

Entity* Layer::entityFromPosition(const sf::Vector2i& mousePos, sf::Vector2f& viewPos)
{
    const auto& window = Application::context().window;
    viewPos = window.mapPixelToCoords(mousePos, m_view);
    return m_root.firstOver(viewPos);
}

void Layer::refreshManipulability()
{
    if (!m_manipulable) {
        m_view.setSize(m_size);
        m_view.setCenter(m_size / 2.f);
    }
}
