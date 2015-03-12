#include "scene/layer.hpp"

#include "core/application.hpp"
#include "resources/identifiers.hpp"

#include <SFML/Graphics/View.hpp>

using namespace scene;

Layer::Layer()
    : m_minZoom(0.2f)
    , m_maxZoom(1.f)
{
    m_root.setDetectable(false);
    m_root.setFocusable(false);
    m_root.setVisible(false);
}

//-----------------------------//
//----- View manipulation -----//

void Layer::setView(ViewID viewID)
{
    m_view = &Application::context().views.get(viewID);
}

void Layer::zoom(float zoomFactor)
{
    m_relZoom *= zoomFactor;
    m_view->zoom(zoomFactor);
    adaptViewZoom();
    adaptViewPosition();
}

void Layer::move(const sf::Vector2f& offset)
{
    m_view->move(offset);
    adaptViewPosition();
}

//--------------------//
//----- Grabbing -----//

void Layer::startGrabbing(const sf::Vector2f& position)
{
    m_grabbingPosition = position;
}

void Layer::moveGrabbing(const sf::Vector2f& position)
{
    move(m_grabbingPosition - position);
}

//------------------//
//----- Limits -----//

void Layer::adaptViewPosition()
{
    const auto& viewHalfSize = m_view->getSize() / 2.f;
    const auto& viewCenter = m_view->getCenter();

    const auto offLeft = (viewCenter.x - viewHalfSize.x) - m_displayRect.left;
    const auto offRight = (viewCenter.x + viewHalfSize.x) - (m_displayRect.left + m_displayRect.width);

    if (offLeft < 0.f) m_view->move(-offLeft, 0.f);
    else if (offRight > 0.f) m_view->move(-offRight, 0.f);

    const auto offTop = (viewCenter.y - viewHalfSize.y) - m_displayRect.top;
    const auto offBottom = (viewCenter.y + viewHalfSize.y) - (m_displayRect.top + m_displayRect.height);

    if (offTop < 0.f) m_view->move(0.f, -offTop);
    else if (offBottom > 0.f) m_view->move(0.f, -offBottom);
}

void Layer::adaptViewZoom()
{
    if (m_relZoom < m_minZoom) {
        m_relZoom = m_minZoom;
        m_view->setSize(m_minSize);
    }
    else if (m_relZoom > m_maxZoom) {
        m_relZoom = m_maxZoom;
        m_view->setSize(m_maxSize);
    }
}

//-------------------//
//----- Changes -----//

void Layer::updateZoomInfo()
{
    const auto& resolution = Application::context().resolution;

    auto viewRatio = resolution.x / resolution.y;
    auto layerRatio = m_displayRect.width / m_displayRect.height;

    // The view reference size is the maximum valid size that
    // does not show anything beyond what displayRect tells.
    sf::Vector2f viewRefSize;
    if (viewRatio < layerRatio) viewRefSize = resolution * layerRatio;
    else viewRefSize = resolution / layerRatio;

    m_minSize = m_minZoom * viewRefSize;
    m_maxSize = m_maxZoom * viewRefSize;

    m_view->setSize(m_maxSize);
    m_view->setCenter({m_displayRect.left + m_displayRect.width / 2.f, m_displayRect.top + m_displayRect.height / 2.f});
    m_relZoom = 1.f;
}
