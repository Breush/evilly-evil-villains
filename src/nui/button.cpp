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

void Button::handleMouseButtonPressed(const sf::Mouse::Button& button, const sf::Vector2f&)
{
    returnif (button != sf::Mouse::Left);

    acceptChoice();
}

bool Button::handleKeyboardEvent(const sf::Event& event)
{
    // Just manage pressed keys and Return
    returnif (event.type != sf::Event::KeyPressed) false;
    returnif (event.key.code != sf::Keyboard::Return) false;

    acceptChoice();
    return true;
}

/*void Button::handleJoystickEvent(const sf::Event&)
{
    // TODO one day, similar to Keyboard
}*/

