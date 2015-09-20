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
    refreshContentStatus();
    refreshBars();
}

void ScrollArea::onChildSizeChanges(scene::Entity&)
{
    refreshContentStatus();
    refreshBars();
}

//------------------//
//----- Events -----//

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
    refreshContentStatus();
    refreshBars();
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
        m_barsLength.x = size().x * freeContentSize.x / m_content->size().x;
    }

    if (freeContentSize.y > 0.f) {
        addPart(&m_vBar);
        m_barsLength.y = size().y * freeContentSize.y / m_content->size().y;
    }

    // Bar offsets
    // TODO Use cNUI for this 10.f
    m_hBar.setSize({m_barsLength.x, 10.f});
    m_vBar.setSize({10.f, m_barsLength.y});
    m_barsOffset = {size().y - 10.f, size().x - 10.f};
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
