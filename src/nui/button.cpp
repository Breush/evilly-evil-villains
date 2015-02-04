#include "nui/button.hpp"

using namespace nui;

Button::Button()
    : baseClass()
{
}

void Button::init()
{
    // Adding unique choice
    add(L"", nullptr);
    setShowArrows(false);

    baseClass::init();
}

//-------------------//
//----- Setters -----//

void Button::setText(const std::wstring& text)
{
    setChoiceText(0, text);
}

void Button::setCallback(const Callback callback)
{
    setChoiceCallback(0, callback);
}

void Button::setAction(const std::wstring& text, const Callback callback)
{
    setText(text);
    setCallback(callback);
}

//------------------//
//----- Events -----//

void Button::handleMousePressed(const sf::Event& event, const sf::Vector2f&, const sf::Vector2f&)
{
    // Just manage left click
    if (event.mouseButton.button != sf::Mouse::Left)
        return;

    acceptChoice();
}

bool Button::handleKeyboardEvent(const sf::Event& event)
{
    // Just manage pressed keys and Return
    if (event.type != sf::Event::KeyPressed)
        return false;

    if (event.key.code != sf::Keyboard::Return)
        return false;

    acceptChoice();
    return true;
}

/*void Button::handleJoystickEvent(const sf::Event&)
{
    // TODO one day, similar to Keyboard
}*/

