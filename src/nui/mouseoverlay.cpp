#include "nui/mouseoverlay.hpp"

#include "context/context.hpp"
#include "tools/string.hpp"

using namespace nui;

MouseOverlay::MouseOverlay()
{
    setDetectable(false);

    addPart(&m_leftText);
    addPart(&m_rightText);
    addPart(&m_mouse);

    // TODO Have it done in refreshNUI()
    m_leftText.setCharacterSize(16);
    m_leftText.setPosition({-20.f, -15.f});
    m_leftText.setFont(context::context.fonts.get("core/global/fonts/nui"));

    m_rightText.setCharacterSize(16);
    m_rightText.setPosition({20.f, -15.f});
    m_rightText.setFont(context::context.fonts.get("core/global/fonts/nui"));

    refreshMouseTexture();
}

//-------------------//
//----- Control -----//

void MouseOverlay::setLeft(const std::wstring& actionName)
{
    m_leftText.setString(actionName);
    refreshMouseTexture();

    auto bounds = boundsSize(m_leftText);
    m_leftText.setOrigin(bounds);
}

void MouseOverlay::setRight(const std::wstring& actionName)
{
    m_rightText.setString(actionName);
    refreshMouseTexture();

    auto bounds = boundsSize(m_rightText);
    m_rightText.setOrigin({0.f, bounds.y});
}

//---------------//
//----- ICU -----//

void MouseOverlay::refreshMouseTexture()
{
    bool left  = !m_leftText.getString().isEmpty();
    bool right = !m_rightText.getString().isEmpty();

    const sf::Texture* texture;
    if      (!left && !right)   texture = &context::context.textures.get("core/cursor/mouse_off");
    else if (!left && right)    texture = &context::context.textures.get("core/cursor/mouse_r");
    else if (left && !right)    texture = &context::context.textures.get("core/cursor/mouse_l");
    else if (left && right)     texture = &context::context.textures.get("core/cursor/mouse_rl");

    const auto textureSize = sf::v2f(texture->getSize());
    m_mouse.setOrigin(textureSize / 2.f);
    m_mouse.setSize(textureSize);
    m_mouse.setTexture(texture);
}
