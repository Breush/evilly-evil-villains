#include "nui/entity.hpp"

#include "core/application.hpp"
#include "tools/string.hpp"
#include "tools/tools.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

using namespace nui;

Entity::Entity(bool isLerpable)
    : baseClass(isLerpable)
{
    // Focusing
    if (sf::Shader::isAvailable())
        m_focusShader = &Application::context().shaders.get("nui/focus");
    m_focusSprite.setTexture(&Application::context().textures.get("nui/focus"));
    m_focusSprite.setFillColor({255, 255, 255, 100});

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

void Entity::onFocusChanged()
{
    // Update the focus sprite
    const auto& focusRectangle = focusRect();
    m_focusSprite.setPosition({focusRectangle.left, focusRectangle.top});
    m_focusSprite.setSize({focusRectangle.width, focusRectangle.height});
}

void Entity::drawInternal(sf::RenderTarget& target, sf::RenderStates states) const
{
    returnif (!focused());

    // Update focus shader
    if (sf::Shader::isAvailable()) {
        const auto& screenSize = Application::context().windowInfo.screenSize;
        auto focusWindowRect = states.transform.transformRect(focusRect());
        focusWindowRect = tools::mapRectCoordsToPixel(target, focusWindowRect);
        focusWindowRect.top = screenSize.y - (focusWindowRect.height + focusWindowRect.top); // SFML - GL compatibility
        m_focusShader->setParameter("position", {focusWindowRect.left, focusWindowRect.top});
        m_focusShader->setParameter("size", {focusWindowRect.width, focusWindowRect.height});
    }

    // And... draw it
    states.shader = m_focusShader;
    target.draw(m_focusSprite, states);
}

void Entity::update(const sf::Time& dt)
{
    // Focus
    if (focused()) {
        // TODO This became affected by game time factor, is it what we want?
        m_focusAnimation += 60.f * dt.asSeconds();
        const auto& focusSize = m_focusSprite.getSize();
        m_focusSprite.setTextureRect(sf::IntRect(-m_focusAnimation, -m_focusAnimation, focusSize.x, focusSize.y));
    }

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
