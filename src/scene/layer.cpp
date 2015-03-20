#include "scene/layer.hpp"

#include "core/application.hpp"
#include "resources/identifiers.hpp"
#include "tools/tools.hpp"
#include "tools/vector.hpp"

#include <SFML/Graphics/View.hpp>

using namespace scene;

Layer::Layer(Graph* graph)
{
    m_root.setGraph(graph);
    m_root.setDetectable(false);
    m_root.setFocusable(false);
    m_root.setVisible(false);

    refreshDisplay();
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

void Layer::refreshDisplay()
{
    const auto& viewport = Application::context().viewport;
    m_view.setViewport(viewport);

    // Set the root entity to the size of the layer.
    // This is used for relative positionning of entities.
    m_root.setSize(m_size);

    if (!m_manipulable) {
        m_view.setSize(m_size);
        m_view.setCenter(m_size / 2.f);
    }

    // Recursively update the whole layer.
    m_root.refreshDisplay();
}

//-----------------------------//
//----- View manipulation -----//

void Layer::setRelativeCenter(const sf::Vector2f& relativeCenter)
{
    m_view.setCenter(m_view.getSize() / 2.f + relativeCenter * (m_size - m_view.getSize()));
}

void Layer::setViewSize(const sf::Vector2f& viewSize)
{
    m_view.setSize(viewSize);
}

Entity* Layer::entityFromPosition(const sf::Vector2i& mousePos, sf::Vector2f& viewPos)
{
    const auto& window = Application::context().window;
    viewPos = window.mapPixelToCoords(mousePos, m_view);
    return m_root.firstOver(viewPos);
}
