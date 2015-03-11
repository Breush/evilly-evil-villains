#include "nui/button.hpp"

#include "tools/tools.hpp"

#include <SFML/Window/Event.hpp>

using namespace nui;

Button::Button()
    : baseClass()
{
    // Adding unique choice
    add(L"", nullptr);
    setShowArrows(false);
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

void Button::handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f&, const sf::Vector2f&)
{
    returnif (button != sf::Mouse::Left);

    acceptChoice();
}

void Button::handleKeyboardEvent(const sf::Event& event)
{
    // Just manage pressed keys and Return
    returnif (event.type != sf::Event::KeyPressed);
    returnif (event.key.code != sf::Keyboard::Return);

    acceptChoice();
}

