#include "nui/pushbutton.hpp"

#include "core/application.hpp"
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
    m_text.setFont(Application::context().fonts.get("core/global/fonts/nui"));
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
    m_imageHintSize = 1.5f * m_fontSize;
    m_text.setCharacterSize(m_fontSize);

    updateSize();
}

void PushButton::updateSize()
{
    sf::Vector2f newSize;
    newSize.x = boundsSize(m_text).x + 2.f * m_hPadding;
    if (m_imageActive) newSize.x += m_imageHintSize + m_hPadding;
    newSize.x = std::max(10.f * m_hPadding, newSize.x);
    newSize.y = m_fontSize + 2.f * m_vPadding;
    setSize(newSize);
    refresh();
}

void PushButton::updateImageTexture()
{
    // Remove image part if it was there before
    if (m_imageActive)
        removePart(&m_image);

    m_imageActive = !m_imageTextureID.empty();

    // Set the texture and add the image part if we need it now
    if (m_imageActive) {
        m_image.setTexture(&Application::context().textures.get(m_imageTextureID));
        addPart(&m_image);
    }

    updateSize();
}

//-------------------//
//----- Setters -----//

void PushButton::setText(const std::wstring& text)
{
    m_text.setString(text);
    updateSize();
}

void PushButton::setImage(const std::string& textureID)
{
    m_imageTextureID = textureID;
    updateImageTexture();
}

void PushButton::setValidateCallback(const ValidateCallback validateCallback)
{
    m_validateCallback = std::move(validateCallback);
    refresh();
}

void PushButton::set(const std::wstring& text, const std::string& imageTextureID)
{
    m_text.setString(text);
    m_imageTextureID = imageTextureID;
    updateImageTexture();
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

    if (m_validateCallback == nullptr) {
        Application::context().sounds.play("core/nui/refuse/refuse");
        return true;
    }

    Application::context().sounds.play("core/nui/accept/accept");
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
    returnif (m_hovered) true;
    returnif (m_validateCallback == nullptr) true;

    Application::context().sounds.play("core/nui/select/select");
    m_hovered = true;

    setPartShader(&m_text, "core/nui/hover/hover");
    setPartShader(&m_left, "core/nui/hover/hover");
    setPartShader(&m_middle, "core/nui/hover/hover");
    setPartShader(&m_right, "core/nui/hover/hover");
    if (m_imageActive) setPartShader(&m_image, "core/nui/hover/hover");
    return true;
}

void PushButton::handleMouseLeft()
{
    m_hovered = false;
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
    if (m_imageActive) m_text.move(0.5f * (m_imageHintSize + m_hPadding), 0.f);

    // Image
    if (m_imageActive) {
        m_image.setSize({m_imageHintSize, m_imageHintSize});
        m_image.setPosition({m_hPadding, 0.45f * size().y});
        m_image.setOrigin({0.f, 0.5f * m_imageHintSize});
    }

    // Button
    const auto& leftTexture =   Application::context().textures.get(m_pressed? "core/nui/pushbutton/push_left"   : "core/nui/pushbutton/pop_left");
    const auto& middleTexture = Application::context().textures.get(m_pressed? "core/nui/pushbutton/push_middle" : "core/nui/pushbutton/pop_middle");
    const auto& rightTexture =  Application::context().textures.get(m_pressed? "core/nui/pushbutton/push_right"  : "core/nui/pushbutton/pop_right");

    float leftTextureWidth = leftTexture.getSize().x;
    float rightTextureWidth = rightTexture.getSize().x;

    m_left.setTexture(&leftTexture);
    m_middle.setTexture(&middleTexture);
    m_right.setTexture(&rightTexture);

    m_left.setPosition(0.f, 0.f);
    m_middle.setPosition(leftTexture.getSize().x, 0.f);
    m_right.setPosition(size().x - rightTexture.getSize().x, 0.f);

    m_left.setSize({leftTextureWidth, size().y});
    m_middle.setSize({size().x - (leftTextureWidth + rightTextureWidth), size().y});
    m_right.setSize({rightTextureWidth, size().y});

    // Activity
    if (m_validateCallback == nullptr) {
        auto grey = sf::Color{120u, 120u, 120u, 255u};
        m_left.setFillColor(grey);
        m_middle.setFillColor(grey);
        m_right.setFillColor(grey);
        m_text.setFillColor(grey);
    }
    else {
        m_left.setFillColor(sf::Color::White);
        m_middle.setFillColor(sf::Color::White);
        m_right.setFillColor(sf::Color::White);
        m_text.setFillColor(sf::Color::White);
    }
}
