#include "nui/pushbutton.hpp"

#include "tools/tools.hpp"
#include "tools/string.hpp"

using namespace nui;

PushButton::PushButton()
    : baseClass()
{
    setFocusable(true);

    // Sprites
    addPart(&m_left);
    addPart(&m_middle);
    addPart(&m_right);

    // Text
    addPart(&m_text);
    m_text.setFont(Application::context().fonts.get("nui"));
}

//-------------------//
//----- Routine -----//

void PushButton::onSizeChanges()
{
    refresh();
}

void PushButton::refreshNUI(const config::NUIGuides& cNUI)
{
    baseClass::refreshNUI(cNUI);

    // Text
    m_fontSize = cNUI.fontSize;
    m_hPadding = cNUI.hPadding;
    m_vPadding = cNUI.vPadding;
    m_text.setCharacterSize(m_fontSize);

    updateSize();
}

void PushButton::updateSize()
{
    sf::Vector2f newSize;
    newSize.x = boundsSize(m_text).x + 2.f * m_hPadding;
    newSize.x = std::max(10.f * m_hPadding, newSize.x);
    newSize.y = m_fontSize + 2.f * m_vPadding;
    setSize(newSize);
    refresh();
}

//-------------------//
//----- Setters -----//

void PushButton::setText(const std::wstring& text)
{
    m_text.setString(text);
    updateSize();
}

void PushButton::setValidateCallback(const ValidateCallback validateCallback)
{
    m_validateCallback = std::move(validateCallback);
    updateSize();
}

void PushButton::set(const std::wstring& text, const ValidateCallback validateCallback)
{
    m_text.setString(text);
    m_validateCallback = std::move(validateCallback);
    updateSize();
}

//------------------//
//----- Events -----//

void PushButton::handleGlobalMouseButtonReleased(const sf::Mouse::Button button, const sf::Vector2f&)
{
    returnif (!m_pressed);
    returnif (button != sf::Mouse::Left);

    m_pressed = false;
    refresh();
}

bool PushButton::handleKeyboardEvent(const sf::Event& event)
{
    returnif (event.type != sf::Event::KeyPressed) false;
    returnif (event.key.code != sf::Keyboard::Return) false;

    validate();
    return true;
}

bool PushButton::handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos)
{
    returnif (button != sf::Mouse::Left) false;

    m_pressed = true;
    refresh();

    return true;
}

bool PushButton::handleMouseButtonReleased(const sf::Mouse::Button button, const sf::Vector2f&, const sf::Vector2f&)
{
    returnif (!m_pressed) false;
    returnif (button != sf::Mouse::Left) false;

    validate();
    return true;
}

bool PushButton::handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos)
{
    resetPartsShader();

    returnif (m_validateCallback == nullptr) true;

    setPartShader(&m_text, "nui/hover");
    return true;
}

void PushButton::handleMouseLeft()
{
    resetPartsShader();
}

//---------------------//
//----- Callbacks -----//

void PushButton::validate()
{
    returnif (m_validateCallback == nullptr);
    m_validateCallback();
}

//---------------//
//----- ICU -----//

void PushButton::refresh()
{
    // Text
    auto textBounds = boundsSize(m_text);
    m_text.setOrigin(0.5f * textBounds.x, m_fontSize);
    m_text.setPosition(0.5f * size().x, m_vPadding + 0.75f * m_fontSize);

    // Button
    const auto& leftTexture = Application::context().textures.get(m_pressed? "nui/pushbutton/push_left" : "nui/pushbutton/pop_left");
    const auto& middleTexture = Application::context().textures.get(m_pressed? "nui/pushbutton/push_middle" : "nui/pushbutton/pop_middle");
    const auto& rightTexture = Application::context().textures.get(m_pressed? "nui/pushbutton/push_right" : "nui/pushbutton/pop_right");

    m_left.setTexture(&leftTexture);
    m_middle.setTexture(&middleTexture);
    m_right.setTexture(&rightTexture);

    m_left.setPosition(0.f, 0.f);
    m_middle.setPosition(leftTexture.getSize().x, 0.f);
    m_right.setPosition(size().x - rightTexture.getSize().x, 0.f);

    m_left.setSize({static_cast<float>(leftTexture.getSize().x), size().y});
    m_middle.setSize({size().x - static_cast<float>(leftTexture.getSize().x + rightTexture.getSize().x), size().y});
    m_right.setSize({static_cast<float>(rightTexture.getSize().x), size().y});
}
