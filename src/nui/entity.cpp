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
        m_focusShader = &Application::context().shaders.get("core/nui/focus/background");
    m_focusSprite.setTexture(&Application::context().textures.get("core/nui/focus/background"));
    m_focusSprite.setFillColor({255, 255, 255, 100});

    // Tooltip
    m_tooltipTime = sf::Time::Zero;
    m_tooltip.setRelativeOrigin({0.5f, 1.05f});
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

void Entity::update(const sf::Time& dt, const float factor)
{
    // Focus
    if (focused()) {
        m_focusAnimation += 60.f * dt.asSeconds();
        const auto& focusSize = m_focusSprite.getSize();
        m_focusSprite.setTextureRect(sf::IntRect(-m_focusAnimation, -m_focusAnimation, focusSize.x, focusSize.y));
    }

    // Tooltip
    if (m_showTooltip && m_tooltipTime < m_tooltipDelay) {
        m_tooltipTime += dt;
        if (m_tooltipTime >= m_tooltipDelay)
            root()->attachChild(m_tooltip);
    }

    baseClass::update(dt, factor);
}

//------------------//
//----- Events -----//

bool Entity::handleMouseMoved(const sf::Vector2f&, const sf::Vector2f& nuiPos)
{
    // Reposition tooltip
    if (m_tooltipEnabled) {
        m_showTooltip = true;
        m_tooltip.setLocalPosition(nuiPos);
    }

    return false;
}

void Entity::handleMouseLeft()
{
    if (m_showTooltip) {
        // We were showing the tooltip...
        if (m_tooltipTime >= m_tooltipDelay)
            root()->detachChild(m_tooltip);

        // Reset
        m_showTooltip = false;
        m_tooltipTime = sf::Time::Zero;
    }
}

//-------------------//
//----- Tooltip -----//

void Entity::setTooltip(const std::wstring& tooltipString)
{
    m_tooltipEnabled = !tooltipString.empty();
    m_tooltip.setText(tooltipString);
}
