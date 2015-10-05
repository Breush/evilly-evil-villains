#include "nui/scrollarea.hpp"

#include "core/application.hpp"
#include "tools/tools.hpp"

#include <iostream>

using namespace nui;

ScrollArea::ScrollArea()
{
    // Bars
    m_hBar.setFillColor(sf::Color::White);
    m_vBar.setFillColor(sf::Color::White);
}

//-------------------//
//----- Routine -----//

void ScrollArea::onSizeChanges()
{
    returnif (m_content == nullptr);
    refreshBars();
    refreshContentStatus();
}

void ScrollArea::onChildSizeChanges(scene::Entity&)
{
    refreshBars();
    refreshContentStatus();
}

void ScrollArea::refreshNUI(const config::NUIGuides& cNUI)
{
    baseClass::refreshNUI(cNUI);

    m_barThick = 2.f * cNUI.largeBorderThick;

    refreshBars();
}

//------------------//
//----- Events -----//

bool ScrollArea::handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    if (m_vBarGrabbed) {
        m_offset.y = m_offsetStartGrabbing.y + (m_mouseStartGrabbing.y - mousePos.y);
        refreshContentStatus();
        return true;
    }

    if (m_hBarGrabbed) {
        m_offset.x = m_offsetStartGrabbing.x + (m_mouseStartGrabbing.x - mousePos.x);
        refreshContentStatus();
        return true;
    }

    return false;
}

bool ScrollArea::handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    returnif (button != sf::Mouse::Left) false;

    // Detect if mouse is over vertical bar
    sf::FloatRect vLocalBounds({0.f, 0.f}, m_vBar.getSize());
    if (vLocalBounds.contains(m_vBar.getInverseTransform().transformPoint(mousePos))) {
        m_offsetStartGrabbing = m_offset;
        m_mouseStartGrabbing = mousePos;
        m_vBarGrabbed = true;
        return true;
    }

    // Detect if mouse is over horizontal bar
    sf::FloatRect hLocalBounds({0.f, 0.f}, m_hBar.getSize());
    if (hLocalBounds.contains(m_hBar.getInverseTransform().transformPoint(mousePos))) {
        m_offsetStartGrabbing = m_offset;
        m_mouseStartGrabbing = mousePos;
        m_hBarGrabbed = true;
        return true;
    }

    return false;
}

bool ScrollArea::handleMouseButtonReleased(const sf::Mouse::Button button, const sf::Vector2f&, const sf::Vector2f&)
{
    returnif (button != sf::Mouse::Left) false;
    m_vBarGrabbed = false;
    m_hBarGrabbed = false;
    return true;
}

void ScrollArea::handleMouseLeft()
{
    m_vBarGrabbed = false;
    m_hBarGrabbed = false;
}

bool ScrollArea::handleMouseWheelMoved(const int delta, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos)
{
    const auto& scrollingFactor = Application::context().display.global.scrollingFactor;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) m_offset += {delta * scrollingFactor, 0.f};
    else m_offset += {0.f, delta * scrollingFactor};
    refreshContentStatus();
    return true;
}

//------------------------------//
//----- Content management -----//

void ScrollArea::setContent(scene::Entity& entity)
{
    m_content = &entity;
    attachChild(*m_content);
    refreshBars();
    refreshContentStatus();
}

//-----------------------------------//
//----- Internal changes update -----//

void ScrollArea::refreshBars()
{
    returnif (m_content == nullptr);

    // Bar visibility and length
    auto freeContentSize = m_content->size() - size();
    clearParts();

    if (freeContentSize.x > 0.f) {
        addPart(&m_hBar);
        m_barsLength.x = size().x * (1.f - freeContentSize.x / m_content->size().x);
    }

    if (freeContentSize.y > 0.f) {
        addPart(&m_vBar);
        m_barsLength.y = size().y * (1.f - freeContentSize.y / m_content->size().y);
    }

    // Bar offsets
    m_hBar.setSize({m_barsLength.x, m_barThick});
    m_vBar.setSize({m_barThick, m_barsLength.y});
    m_barsOffset = {size().y - m_barThick, size().x - m_barThick};
}

void ScrollArea::refreshContentStatus()
{
    returnif (m_content == nullptr);

    auto freeContentSize = m_content->size() - size();
    m_offset.x = std::max(m_offset.x, -freeContentSize.x);
    m_offset.y = std::max(m_offset.y, -freeContentSize.y);
    m_offset.x = std::min(m_offset.x, 0.f);
    m_offset.y = std::min(m_offset.y, 0.f);

    // Set content position
    m_content->setLocalPosition(m_offset);
    m_content->setClipArea({-m_offset.x, -m_offset.y, size().x, size().y});

    // Recompute bars position
    auto barsPosition = -(size() - m_barsLength) * m_offset / freeContentSize;
    m_hBar.setPosition({barsPosition.x, m_barsOffset.x});
    m_vBar.setPosition({m_barsOffset.y, barsPosition.y});
}
