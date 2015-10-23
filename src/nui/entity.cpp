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
}
//-------------------//
//----- Routine -----//

void Entity::update(const sf::Time& dt)
{
    // Tooltip
    if (m_showTooltip)
        m_tooltipTime += dt;

    baseClass::update(dt);
}

void Entity::refreshNUI(const config::NUIGuides& cNUI)
{
    m_tooltipText.setCharacterSize(cNUI.fontSize);
    refreshTooltipBackground();
}

void Entity::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    baseClass::draw(target, states);

    // Tooltip
    if (m_showTooltip && m_tooltipTime >= m_tooltipDelay) {
        states.transform = getTransform();
        target.draw(m_tooltipBackground, states);
        target.draw(m_tooltipText, states);
    }
}

//------------------//
//----- Events -----//

bool Entity::handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos)
{
    // Position and show tooltip
    if (!m_showTooltip) {
        m_showTooltip = true;
        auto textSize = boundsSize(m_tooltipText);
        auto backgroundSize = m_tooltipBackground.getSize();
        m_tooltipText.setPosition(mousePos.x - textSize.x / 2.f, mousePos.y - textSize.y - 8.f);
        m_tooltipBackground.setPosition(mousePos.x - backgroundSize.x / 2.f, mousePos.y - backgroundSize.y);
    }

    return false;
}

void Entity::handleMouseLeft()
{
    m_showTooltip = false;
    m_tooltipTime = sf::Time::Zero;
}

//-------------------//
//----- Tooltip -----//

void Entity::setTooltip(std::wstring tooltipString)
{
    m_tooltipText.setString(tooltipString);
    m_tooltipText.setFont(Application::context().fonts.get("nui"));
    m_tooltipText.setColor(sf::Color::White);
    refreshTooltipBackground();
}

//-----------------------------------//
//----- Internal change updates -----//

void Entity::refreshTooltipBackground()
{
    auto tooltipSize = boundsSize(m_tooltipText);
    m_tooltipBackground.setSize({tooltipSize.x + 10.f, tooltipSize.y + 10.f});
}
