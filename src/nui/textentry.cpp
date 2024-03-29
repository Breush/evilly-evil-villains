#include "nui/textentry.hpp"

#include "context/context.hpp"
#include "tools/vector.hpp"
#include "tools/string.hpp"
#include "tools/tools.hpp"

#include <SFML/Window/Event.hpp>

using namespace nui;

TextEntry::TextEntry()
    : m_width(0.f)
    , m_length(-1u)
    , m_maxCharacters(40u)
{
    setFocusable(true);

    // Background
    addPart(&m_backgroundLeft);
    addPart(&m_backgroundRight);
    addPart(&m_backgroundMiddle);

    m_backgroundLeft.setTexture(&context::context.textures.get("core/nui/textentry/background_left"));
    m_backgroundRight.setTexture(&context::context.textures.get("core/nui/textentry/background_right"));
    m_backgroundMiddle.setTexture(&context::context.textures.get("core/nui/textentry/background_middle"));

    addPart(&m_selectRect);
    m_selectRect.setFillColor({220u, 180u, 160u, 160u});

    addPart(&m_text);
    m_text.setFont(context::context.fonts.get("core/global/fonts/nui"));
    m_text.setFillColor(sf::Color::White);

    addPart(&m_cursor);
    m_cursor.setShade(0.f);
    m_cursor.setColor(sf::Color::White);

    m_cursorText = m_text;
    m_selectText = m_text;
}

//-------------------//
//----- Routine -----//

void TextEntry::onSizeChanges()
{
    // Background
    auto backgroundLeftTextureSize = sf::v2f(m_backgroundLeft.getTexture()->getSize());
    auto backgroundRightTextureSize = sf::v2f(m_backgroundRight.getTexture()->getSize());
    auto backgroundMiddleTextureSize = sf::v2f(m_backgroundMiddle.getTexture()->getSize());
    float scaleFactor = size().y / backgroundMiddleTextureSize.y;

    m_backgroundLeft.setSize(backgroundLeftTextureSize * scaleFactor);
    m_backgroundRight.setSize(backgroundLeftTextureSize * scaleFactor);
    m_backgroundMiddle.setSize({size().x - (backgroundLeftTextureSize.x + backgroundRightTextureSize.x) * scaleFactor, size().y});

    m_backgroundMiddle.setPosition(backgroundLeftTextureSize.x * scaleFactor, 0.f);
    m_backgroundRight.setPosition(size().x - backgroundRightTextureSize.x * scaleFactor, 0.f);

    // Cursor
    float vOffset = -m_textPadding / 2.f;
    m_cursor.setLength(size().y - m_textPadding);
    m_cursor.setOrigin(0.f, vOffset);

    // Text
    m_textWidthLimit = size().x - 2.f * m_textPadding;
    setPartClippingRect(&m_text, {m_textPadding, 0.f, m_textWidthLimit, size().y});
    m_text.setOrigin(0.f, vOffset);

    // Select rect
    setPartClippingRect(&m_selectRect, {m_textPadding, 0.f, m_textWidthLimit, size().y});
    m_selectRect.setOrigin(0.f, vOffset);


    updateDynamicText();
}

void TextEntry::updateSize()
{
    if (m_length != -1u)
        m_width = m_length * m_fontHSpace + 2.f * m_textPadding;

    setSize({m_width, m_fontVSpace + m_textPadding});
}

void TextEntry::updateRoutine(const sf::Time& dt)
{
    // Do not blink if not focused
    if (!focused()) {
        if (m_cursorOn) {
            removePart(&m_cursor);
            m_cursorOn = false;
        }
        return;
    }

    // Blinking cursor if focus is on
    m_cursorBlinkTime += dt.asSeconds();
    while (m_cursorBlinkTime >= 0.5f) {
        m_cursorBlinkTime -= 0.5f;
        if (m_cursorOn) removePart(&m_cursor);
        else addPart(&m_cursor);
        m_cursorOn = !m_cursorOn;
    }
}

void TextEntry::refreshNUI(const config::NUIGuides& cNUI)
{
    baseClass::refreshNUI(cNUI);

    m_text.setCharacterSize(cNUI.fontSize);
    m_cursorText.setCharacterSize(cNUI.fontSize);
    m_selectText.setCharacterSize(cNUI.fontSize);

    m_textPadding = cNUI.hPadding + cNUI.vPadding;
    m_fontVSpace = cNUI.fontVSpace;
    m_fontHSpace = cNUI.fontHSpace;

    updateSize();
}

//------------------//
//----- Events -----//

bool TextEntry::handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    returnif (button != sf::Mouse::Left) false;

    deselect();
    m_selecting = true;

    m_selectStart = getCharacterPosition(mousePos.x, m_cursorText);
    m_selectShiftStart = boundsSize(m_cursorText).x;
    m_cursorString = m_cursorText.getString();

    updateDynamicText();
    return true;
}

bool TextEntry::handleMouseButtonReleased(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    returnif (button != sf::Mouse::Left) false;
    m_selecting = false;
    return true;
}

bool TextEntry::handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    returnif (!m_selecting) false;

    uint selectEnd = getCharacterPosition(mousePos.x, m_cursorText);
    m_cursorString = m_cursorText.getString();
    select(m_selectStart, selectEnd);

    updateDynamicText();
    return true;
}

bool TextEntry::handleKeyboardEvent(const sf::Event& event)
{
    // Left/right to move inside the text entry
    // Use Ctrl to skip a word
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Left) {
            int toMove = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)? previousRelativeWord() : -1;
            moveCursor(toMove);
            return true;
        }
        else if (event.key.code == sf::Keyboard::Right) {
            int toMove = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)? nextRelativeWord() : 1;
            moveCursor(toMove);
            return true;
        }
        // Select all with Ctrl-A
        else if (event.key.code == sf::Keyboard::A && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
            selectAll();
        }
    }
    // Edit the text entry
    else if (event.type == sf::Event::TextEntered) {
        returnif (event.text.unicode == 1) false;           // Error
        returnif (event.text.unicode == 27) false;          // Escape
        returnif (event.text.unicode == 9) false;           // Tab
        if (event.text.unicode == 13) {                     // Return
            if (m_onValidateCallback != nullptr)
                m_onValidateCallback();
        }
        else if (event.text.unicode == 8) deletePrevious(); // Backspace
        else if (event.text.unicode == 127) deleteNext();   // Delete
        else addCharacter(event.text.unicode);              // Any correct character
        return true;
    }

    return false;
}

void TextEntry::handleMouseLeft()
{
    m_selecting = false;
}

//-------------------------//
//----- Getter/setter -----//

void TextEntry::setText(const std::wstring& str, bool sendCallback)
{
    returnif (m_textString == str);
    m_cursorString = str;
    m_textString = str;
    m_cursorText.setString(m_cursorString);
    refreshText(sendCallback);
    deselect();
}

void TextEntry::setOnTextChangeCallback(const TextChangeCallback& callback)
{
    m_onTextChangeCallback = callback;
}

void TextEntry::setOnValidateCallback(const ValidateCallback& callback)
{
    m_onValidateCallback = callback;
}

void TextEntry::setCursorPosition(uint position)
{
    returnif (position > m_textString.getSize());

    // Deselect if any
    deselect();

    // Reset to 0 and move
    m_cursorString = L"";
    moveCursor(position);
}

//------------------//
//----- Delete -----//

bool TextEntry::deleteSelection(bool startRefresh)
{
    returnif (m_selectString.isEmpty()) false;

    if (m_selectL2R) {
        m_cursorString = m_textString.substring(0u, m_selectStart);
        m_textString.erase(m_selectStart, m_selectString.getSize());
    }
    else {
        m_cursorString = m_textString.substring(0u, m_selectStart - m_selectString.getSize());
        m_textString.erase(m_selectStart - m_selectString.getSize(), m_selectString.getSize());
    }

    m_cursorText.setString(m_cursorString);
    deselect();

    if (startRefresh)
        refreshText(true);

    return true;
}

void TextEntry::deletePrevious()
{
    returnif (deleteSelection());
    returnif (m_cursorString.isEmpty());

    int toRemove = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)? previousRelativeWord() : -1;
    m_textString.erase(m_cursorString.getSize() + toRemove, -toRemove);
    moveCursor(toRemove);
    refreshText(true);
}

void TextEntry::deleteNext()
{
    returnif (deleteSelection());
    returnif (m_cursorString == m_textString);

    int toRemove = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)? nextRelativeWord() : 1;
    m_textString.erase(m_cursorString.getSize(), toRemove);
    refreshText(true);
}

//---------------------//
//----- Selection -----//

void TextEntry::selectAll()
{
    deselect();
    select(0u, m_textString.getSize());
}

void TextEntry::select(uint start, uint end)
{
    m_selectL2R = (start < end);
    if (m_selectL2R) m_selectString = m_textString.substring(start, end - start);
    else  m_selectString = m_textString.substring(end, start - end);
    refreshSelection();
}

bool TextEntry::deselect()
{
    m_selectShiftStart = 0.f;
    m_selectStart = 0.f;

    returnif (m_selectString.isEmpty()) false;

    m_selecting = false;
    m_selectString.clear();
    m_selectRect.setSize({0.f, 0.f});
    refreshSelection();
    return true;
}

//------------------//
//----- Cursor -----//

uint TextEntry::getCharacterPosition(const float relX, sf::Text& text)
{
    sf::String string;
    float prevWidth = 0.f;
    float correctX = relX + m_textShift - m_textPadding;

    // Special case: click before the string
    if (relX <= m_textPadding) {
        text.setString(string);
        return 0u;
    }

    for (uint i = 0u; i < m_textString.getSize(); ++i) {
        string += m_textString[i];
        text.setString(string);

        // We just hit the next character,
        // need to determine if we are closer to the previous one or not
        float width = boundsSize(text).x;
        if (width >= correctX) {
            if (width - correctX > correctX - prevWidth) {
                text.setString(string.substring(0u, string.getSize() - 1u));
                return i;
            }
            else return i + 1u;
        }

        prevWidth = width;
    }

    return m_textString.getSize();
}

void TextEntry::addCharacter(const uint32_t character)
{
    deleteSelection(false);

    returnif (m_textString.getSize() == m_maxCharacters);
    m_textString = m_cursorString + character + m_textString.substring(m_cursorString.getSize());
    moveCursor(1);
    deselect();
    refreshText(true);
}

void TextEntry::moveCursor(int relativePos)
{
    bool shiftPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift);
    returnif (!shiftPressed && deselect());
    returnif (relativePos == 0);
    returnif (relativePos < 0 && m_cursorString.isEmpty());
    returnif (relativePos > 0 && m_cursorString == m_textString);

    // On shift, start selection if not already
    if (shiftPressed && m_selectString.isEmpty()) {
        m_selectStart = m_cursorString.getSize();
        m_selectShiftStart = boundsSize(m_cursorText).x;
    }

    auto cursorStringSize = m_cursorString.getSize() + relativePos;
    m_cursorString = m_textString;
    m_cursorString.erase(cursorStringSize, m_textString.getSize() - cursorStringSize);
    m_cursorText.setString(m_cursorString);

    // On shift, also add to selection
    if (shiftPressed) {
        uint selectEnd = m_cursorString.getSize();
        select(m_selectStart, selectEnd);
    }

    updateDynamicText();
}

int TextEntry::previousRelativeWord()
{
    int cursorStringSize = m_cursorString.getSize() - 1;

    int i = 0;
    for (; i < cursorStringSize; ++i)
        if (iswalnum(m_cursorString[cursorStringSize - i])) break;
    for (; i < cursorStringSize; ++i)
        returnif (!iswalnum(m_cursorString[cursorStringSize - i])) -i;

    return -(cursorStringSize + 1);
}

int TextEntry::nextRelativeWord()
{
    int cursorStringSize = m_cursorString.getSize();
    int textStringSize = m_textString.getSize() - 1;

    int i = 0;
    for (; i <= textStringSize - cursorStringSize; ++i)
        if (!iswalnum(m_textString[cursorStringSize + i])) break;
    for (; i <= textStringSize - cursorStringSize; ++i)
        returnif (iswalnum(m_textString[cursorStringSize + i])) i;

    return textStringSize - cursorStringSize + 1;
}

//-----------------------------------//
//----- Internal change updates -----//

void TextEntry::refreshText(bool sendCallback)
{
    if (sendCallback && m_onTextChangeCallback != nullptr)
        m_onTextChangeCallback(m_text.getString().toWideString(), m_textString.toWideString());

    m_text.setString(m_textString);
    updateDynamicText();
}

void TextEntry::refreshSelection()
{
    m_selectText.setString(m_selectString);
    m_selectRect.setSize({boundsSize(m_selectText).x, size().y - m_textPadding});
    refreshSelectionPosition();
}

void TextEntry::refreshSelectionPosition()
{
    if (m_selectL2R) m_selectRect.setPosition(m_textPadding + m_selectShiftStart - m_textShift, 0.f);
    else m_selectRect.setPosition(m_textPadding - boundsSize(m_selectText).x + m_selectShiftStart - m_textShift, 0.f);
}

void TextEntry::updateDynamicText()
{
    const auto& textSize = boundsSize(m_text);
    const auto& cursorTextSize = boundsSize(m_cursorText);

    // Following the cursor if needed, e.g. text is bigger than allowed text size
    if (textSize.x >= m_textWidthLimit) {
        // Special case, stick to the right when cursor is at the end of text
        if (m_textString == m_cursorString)
            m_textShift = textSize.x - m_textWidthLimit;
        // Cursor is now too much to the right
        else if (cursorTextSize.x - m_textShift > m_textWidthLimit)
            m_textShift = cursorTextSize.x - m_textWidthLimit;
        // Cursor is now too much to the left
        else if (cursorTextSize.x - m_textShift < 0.f)
            m_textShift = cursorTextSize.x;
    }
    // Fits OK
    else {
        m_textShift = 0.f;
    }

    // Positions
    m_text.setPosition(m_textPadding - m_textShift, 0.f);
    m_cursor.setPosition({m_textPadding - m_textShift + cursorTextSize.x + 0.1f * m_textPadding, 0.f});
    refreshSelectionPosition();
}

