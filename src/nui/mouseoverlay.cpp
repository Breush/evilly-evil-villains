#include "nui/mouseoverlay.hpp"

#include "core/application.hpp"
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
    m_leftText.setFont(Application::context().fonts.get("nui"));

    m_rightText.setCharacterSize(16);
    m_rightText.setPosition({20.f, -15.f});
    m_rightText.setFont(Application::context().fonts.get("nui"));

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
    if      (!left && !right)   texture = &Application::context().textures.get("global/mouse_off");
    else if (!left && right)    texture = &Application::context().textures.get("global/mouse_r");
    else if (left && !right)    texture = &Application::context().textures.get("global/mouse_l");
    else if (left && right)     texture = &Application::context().textures.get("global/mouse_rl");


    const auto textureSize = sf::v2f(texture->getSize());
    m_mouse.setOrigin(textureSize / 2.f);
    m_mouse.setSize(textureSize);
    m_mouse.setTexture(texture);
}
