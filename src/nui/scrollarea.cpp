#include "nui/scrollarea.hpp"

#include "core/application.hpp"
#include "tools/tools.hpp"

#include <iostream>

using namespace nui;

ScrollArea::ScrollArea()
{
    // Bars
    const auto& vBarStartTexture = Application::context().textures.get("nui/scrollarea/vbar_start");
    const auto& vBarMiddleTexture = Application::context().textures.get("nui/scrollarea/vbar_middle");
    const auto& vBarEndTexture = Application::context().textures.get("nui/scrollarea/vbar_end");
    m_vBarStart.setTexture(vBarStartTexture);
    m_vBarMiddle.setTexture(&vBarMiddleTexture);
    m_vBarEnd.setTexture(vBarEndTexture);
    m_vBarMiddle.setOrigin(0.f, -static_cast<float>(vBarStartTexture.getSize().y));
    m_vBarEnd.setOrigin(0.f, static_cast<float>(vBarEndTexture.getSize().y));

    const auto& hBarStartTexture = Application::context().textures.get("nui/scrollarea/hbar_start");
    const auto& hBarMiddleTexture = Application::context().textures.get("nui/scrollarea/hbar_middle");
    const auto& hBarEndTexture = Application::context().textures.get("nui/scrollarea/hbar_end");
    m_hBarStart.setTexture(hBarStartTexture);
    m_hBarMiddle.setTexture(&hBarMiddleTexture);
    m_hBarEnd.setTexture(hBarEndTexture);
    m_hBarMiddle.setOrigin(-static_cast<float>(hBarStartTexture.getSize().x), 0.f);
    m_hBarEnd.setOrigin(static_cast<float>(hBarEndTexture.getSize().x), 0.f);

    m_barMiddleThick = vBarMiddleTexture.getSize().x;
    m_barMiddleStep = vBarStartTexture.getSize().y + vBarEndTexture.getSize().y;

    // Grabbers
    const auto vGrabberTextureString = "nui/scrollarea/vgrabber";
    const auto& vGrabberTexture = Application::context().textures.get(vGrabberTextureString);
    m_vGrabberSize = sf::v2f(vGrabberTexture.getSize());
    m_vGrabber.setTexture(vGrabberTextureString);
    m_hGrabber.setTexture(vGrabberTextureString);
    m_hGrabber.setLocalRotation(90.f);
    m_hGrabber.setOrigin({0.f, static_cast<float>(vGrabberTexture.getSize().y)});

    attachChild(m_vGrabber);
    attachChild(m_hGrabber);
    m_vGrabber.setVisible(false);
    m_hGrabber.setVisible(false);
    m_vGrabber.setDepth(-50.f);
    m_hGrabber.setDepth(-50.f);
}

//-------------------//
//----- Routine -----//

void ScrollArea::onSizeChanges()
{
    returnif (m_content == nullptr);
    refreshBars();
    refreshContentStatus();
}

void ScrollArea::onChildSizeChanges(scene::Entity& entity)
{
    returnif (m_content != &entity);

    refreshBars();
    refreshContentStatus();
}

void ScrollArea::refreshNUI(const config::NUIGuides& cNUI)
{
    baseClass::refreshNUI(cNUI);
    // TODO Remember and use some NUI size factor
    // refreshBars();
}

//------------------//
//----- Events -----//

void ScrollArea::handleGlobalMouseMoved(const sf::Vector2f& nuiPos)
{
    if (m_vBarGrabbed || m_hBarGrabbed)
        refreshOffsetFromPosition(nuiPos);
}

void ScrollArea::handleGlobalMouseButtonReleased(const sf::Mouse::Button button, const sf::Vector2f&)
{
    returnif (button != sf::Mouse::Left);
    m_vBarGrabbed = false;
    m_hBarGrabbed = false;
}

bool ScrollArea::handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos)
{
    returnif (button != sf::Mouse::Left) false;

    // Detect if mouse is over vertical bar
    if (m_vGrabber.visible() && mousePos.x >= m_availableSize.x) {
        m_vBarGrabbed = true;
        refreshOffsetFromPosition(nuiPos);
        return true;
    }

    // Detect if mouse is over horizontal bar
    if (m_hGrabber.visible() && mousePos.y >= m_availableSize.y) {
        m_hBarGrabbed = true;
        refreshOffsetFromPosition(nuiPos);
        return true;
    }

    return false;
}

bool ScrollArea::handleMouseWheelMoved(const int delta, const sf::Vector2f&, const sf::Vector2f&)
{
    const auto& scrollingFactor = Application::context().display.global.scrollingFactor;
    const float offset = delta * scrollingFactor;

    // Scroll in the direction of scrolling if there is just one
    if (m_hGrabber.visible() && !m_vGrabber.visible())
        m_offset += {offset, 0.f};
    else if (!m_hGrabber.visible() && m_vGrabber.visible())
        m_offset += {0.f, offset};

    // Else, the user can use shift to scroll horizontally
    else {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) m_offset += {offset, 0.f};
        else m_offset += {0.f, offset};
    }

    refreshContentStatus();
    return true;
}

void ScrollArea::handleMouseLeft()
{
    if (m_mouseLeftCallback != nullptr)
        m_mouseLeftCallback();
}

//------------------------------//
//----- Content management -----//

void ScrollArea::setContent(scene::Entity& entity)
{
    if (m_content != nullptr) detachChild(*m_content);
    m_content = &entity;
    attachChild(*m_content);

    refreshBars();
    refreshContentStatus();
}

//---------------------//
//----- Callbacks -----//

void ScrollArea::setMouseLeftCallback(MouseLeftCallback mouseLeftCallback)
{
    m_mouseLeftCallback = mouseLeftCallback;
}

//-----------------------------------//
//----- Internal changes update -----//

void ScrollArea::refreshOffsetFromPosition(const sf::Vector2f& nuiPos)
{
    auto relPos = getInverseTransform().transformPoint(nuiPos);
    auto freeContentSize = m_content->size() - m_availableSize;

    if (m_vBarGrabbed) {
        auto vBarPosition = relPos.y - m_vGrabberSize.y / 2.f;
        auto vPercent = vBarPosition / (m_availableSize.y - m_vGrabberSize.y);
        m_offset.y = - vPercent * freeContentSize.y;
    }

    else if (m_hBarGrabbed) {
        auto hBarPosition = relPos.x - m_vGrabberSize.y / 2.f;
        auto hPercent = hBarPosition / (m_availableSize.x - m_vGrabberSize.y);
        m_offset.x = - hPercent * freeContentSize.x;
    }

    refreshContentStatus();
}

void ScrollArea::refreshBars()
{
    returnif (m_content == nullptr);

    clearParts();
    m_vGrabber.setVisible(false);
    m_hGrabber.setVisible(false);

    // Bar visibility
    auto freeContentSize = m_content->size() - size() - m_vGrabberSize.x;
    m_availableSize = size();

    if (freeContentSize.x > 0.f) {
        addPart(&m_hBarStart);
        addPart(&m_hBarMiddle);
        addPart(&m_hBarEnd);
        m_hGrabber.setVisible(true);
        m_availableSize.y -= m_vGrabberSize.x;
    }

    if (freeContentSize.y > 0.f) {
        addPart(&m_vBarStart);
        addPart(&m_vBarMiddle);
        addPart(&m_vBarEnd);
        m_vGrabber.setVisible(true);
        m_availableSize.x -= m_vGrabberSize.x;
    }

    // Bar lengths
    m_vBarMiddle.setSize({m_barMiddleThick, m_availableSize.y - m_barMiddleStep});
    m_hBarMiddle.setSize({m_availableSize.x - m_barMiddleStep, m_barMiddleThick});

    // Bar offsets
    m_vBarStart.setPosition({m_availableSize.x, 0.f});
    m_vBarMiddle.setPosition({m_availableSize.x, 0.f});
    m_vBarEnd.setPosition(m_availableSize);

    m_hBarStart.setPosition({0.f, m_availableSize.y});
    m_hBarMiddle.setPosition({0.f, m_availableSize.y});
    m_hBarEnd.setPosition(m_availableSize);
}

void ScrollArea::refreshContentStatus()
{
    returnif (m_content == nullptr);

    auto freeContentSize = m_content->size() - m_availableSize;
    m_offset.x = std::max(m_offset.x, -freeContentSize.x);
    m_offset.y = std::max(m_offset.y, -freeContentSize.y);
    m_offset.x = std::min(m_offset.x, 0.f);
    m_offset.y = std::min(m_offset.y, 0.f);

    // Set content position
    m_content->setLocalPosition(m_offset);
    m_content->setClipArea({-m_offset.x, -m_offset.y, m_availableSize.x, m_availableSize.y});

    // Recompute grabbers position
    auto barsPosition = - (m_availableSize - m_vGrabberSize.y) * m_offset / freeContentSize;
    m_hGrabber.setLocalPosition({barsPosition.x, m_availableSize.y});
    m_vGrabber.setLocalPosition({m_availableSize.x, barsPosition.y});
}
