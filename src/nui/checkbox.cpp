#include "nui/checkbox.hpp"

#include "core/application.hpp"
#include "tools/tools.hpp"

using namespace nui;

CheckBox::CheckBox()
    : baseClass()
{
    setFocusable(true);

    // Sprite
    addPart(&m_sprite);
    updateSprite();
}

//-------------------//
//----- Routine -----//

void CheckBox::onSizeChanges()
{
    m_sprite.setSize(size());
}

void CheckBox::refreshNUI(const config::NUIGuides& cNUI)
{
    baseClass::refreshNUI(cNUI);

    // TODO: We should adapt the size to the NUI factor
    // updateSize();
}

//------------------//
//----- Status -----//

void CheckBox::setStatus(bool status)
{
    returnif (m_status == status);
    m_status = status;
    updateSprite();
}

//------------------//
//----- Events -----//

bool CheckBox::handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos)
{
    returnif (button != sf::Mouse::Left) false;

    setStatus(!m_status);

    return true;
}

bool CheckBox::handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos)
{
    setPartShader(&m_sprite, "nui/hover");
    return true;
}

void CheckBox::handleMouseLeft()
{
    resetPartsShader();
}

//---------------------------//
//----- Keyboard events -----//

bool CheckBox::handleKeyboardEvent(const sf::Event& event)
{
    // Just pressing Return is interesting
    returnif (event.type != sf::Event::KeyPressed) false;
    returnif (event.key.code != sf::Keyboard::Return) false;

    setStatus(!m_status);

    return true;
}

//---------------//
//----- ICU -----//

void CheckBox::updateSprite()
{
    const auto& texture = Application::context().textures.get(m_status? "nui/checkbox/on" : "nui/checkbox/off");
    m_sprite.setTexture(&texture);
    setSize(sf::v2f(texture.getSize()));
}
