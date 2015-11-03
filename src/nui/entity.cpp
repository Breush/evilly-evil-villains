#include "nui/entity.hpp"

#include "core/application.hpp"
#include "tools/string.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

using namespace nui;

Entity::Entity(bool isLerpable)
    : baseClass(isLerpable)
{
    // Tooltip
    m_tooltipTime = sf::Time::Zero;

    m_tooltipBackground.setFillColor({0u, 0u, 0u, 222u});
    m_tooltipBackground.setOutlineColor(sf::Color::White);
    m_tooltipBackground.setOutlineThickness(1.f);
    m_tooltipBackground.setDepth(-998.f);

    m_tooltipText.setFont("nui");
    m_tooltipText.setColor(sf::Color::White);
    m_tooltipText.setDepth(-999.f);
}

//-------------------//
//----- Routine -----//

void Entity::update(const sf::Time& dt)
{
    // Tooltip
    if (m_showTooltip && m_tooltipTime < m_tooltipDelay) {
        m_tooltipTime += dt;

        if (m_tooltipTime >= m_tooltipDelay) {
            root()->attachChild(m_tooltipBackground);
            root()->attachChild(m_tooltipText);
        }
    }

    baseClass::update(dt);
}

void Entity::refreshNUI(const config::NUIGuides& cNUI)
{
    m_tooltipText.setCharacterSize(cNUI.fontSize);
    refreshTooltipBackground();
}

//------------------//
//----- Events -----//

bool Entity::handleMouseMoved(const sf::Vector2f&, const sf::Vector2f& nuiPos)
{
    // Reposition tooltip
    if (m_tooltipEnabled) {
        m_showTooltip = true;
        auto textSize = m_tooltipText.size();
        auto backgroundSize = m_tooltipBackground.size();
        m_tooltipText.setLocalPosition({nuiPos.x - textSize.x / 2.f, nuiPos.y - textSize.y - 13.f});
        m_tooltipBackground.setLocalPosition({nuiPos.x - backgroundSize.x / 2.f, nuiPos.y - backgroundSize.y - 5.f});
    }

    return false;
}

void Entity::handleMouseLeft()
{
    if (m_showTooltip) {
        // We were showing the tooltip...
        if (m_tooltipTime >= m_tooltipDelay) {
            root()->detachChild(m_tooltipBackground);
            root()->detachChild(m_tooltipText);
        }

        // Reset
        m_showTooltip = false;
        m_tooltipTime = sf::Time::Zero;
    }
}

//-------------------//
//----- Tooltip -----//

void Entity::setTooltip(std::wstring tooltipString)
{
    m_tooltipEnabled = !tooltipString.empty();
    m_tooltipText.setText(tooltipString);
    refreshTooltipBackground();
}

//-----------------------------------//
//----- Internal change updates -----//

void Entity::refreshTooltipBackground()
{
    auto tooltipSize = m_tooltipText.size();
    m_tooltipBackground.setSize({tooltipSize.x + 10.f, tooltipSize.y + 10.f});
}
