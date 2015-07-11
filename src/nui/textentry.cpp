#include "nui/textentry.hpp"

#include "core/application.hpp"
#include "resources/identifiers.hpp"
#include "config/nui.hpp"
#include "tools/vector.hpp"

using namespace nui;

TextEntry::TextEntry()
    : m_length(0u)
    , m_maxCharacters(40u)
{
    setFocusable(true);

    addPart(&m_background);
    m_background.setFillColor(sf::Color::White);

    addPart(&m_text);
    m_text.setFont(Application::context().fonts.get(FontID::NUI));
    m_text.setColor(sf::Color::Black);

    addPart(&m_cursor);
    m_cursor.setShade(0.f);
    m_cursor.setColor(sf::Color::Black);

    addPart(&m_selectRect);
    m_selectRect.setFillColor({100u, 60u, 40u, 60u});

    m_cursorText = m_text;
    m_selectText = m_text;

    refreshDisplay();
}

//-------------------//
//----- Routine -----//

void TextEntry::onSizeChanges()
{
    m_background.setSize(size());
    m_cursor.setLength(size().y - m_textPadding);

    m_textWidthLimit = size().x - 2.f * m_textPadding;

    setPartClippingRect(&m_text, {m_textPadding, 0.f, m_textWidthLimit, size().y});
    setPartClippingRect(&m_selectRect, {m_textPadding, 0.f, m_textWidthLimit, size().y});
    updateDynamicText();
}

void TextEntry::updateSize()
{
    setSize({m_length * m_fontHSpace + 2.f * m_textPadding, m_fontVSpace + 2.f * m_textPadding});
}

void TextEntry::updateRoutine(const sf::Time& dt)
{
    returnif (!focused());

    // Blinking cursor if focus is on
    m_cursorBlinkTime += dt.asSeconds();
    while (m_cursorBlinkTime >= 0.5f) {
        m_cursorBlinkTime -= 0.5f;
        if (m_cursorOn) removePart(&m_cursor);
        else addPart(&m_cursor);
        m_cursorOn = !m_cursorOn;
    }
}

void TextEntry::refreshDisplay()
{
    config::NUI cNUI;

    m_text.setCharacterSize(cNUI.fontSize);
    m_cursorText.setCharacterSize(cNUI.fontSize);
    m_selectText.setCharacterSize(cNUI.fontSize);

    m_textPadding = (cNUI.hPadding + cNUI.vPadding) / 2.f;
    m_fontVSpace = cNUI.fontVSpace;
    m_fontHSpace = cNUI.fontHSpace;

    updateSize();
    baseClass::refreshDisplay();
}

//------------------//
//----- Events -----//

void TextEntry::handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    returnif (button != sf::Mouse::Left);

    deselect();
    m_selecting = true;

    m_selectStart = getCharacterPosition(mousePos.x, m_cursorText);
    m_selectShiftStart = boundsSize(m_cursorText).x;
    m_cursorString = m_cursorText.getString();

    updateDynamicText();
}

void TextEntry::handleMouseButtonReleased(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    returnif (button != sf::Mouse::Left);
    m_selecting = false;
}

void TextEntry::handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    returnif (!m_selecting);

    uint selectEnd = getCharacterPosition(mousePos.x, m_cursorText);
    m_cursorString = m_cursorText.getString();
    select(m_selectStart, selectEnd);

    updateDynamicText();
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
        // TODO Associate a callback to Enter?
        returnif (event.text.unicode == 1) false;           // Error
        returnif (event.text.unicode == 13) false;          // Enter
        if (event.text.unicode == 8) deletePrevious();      // Backspace
        else if (event.text.unicode == 127) deleteNext();   // Delete
        else addCharacter(event.text.unicode);              // Any correct character
        return true;
    }

    return false;
}

//------------------//
//----- Delete -----//

bool TextEntry::deleteSelection()
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
    m_text.setString(m_textString);

    updateDynamicText();
    deselect();
    return true;
}

void TextEntry::deletePrevious()
{
    returnif (deleteSelection());
    returnif (m_cursorString.isEmpty());

    int toRemove = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)? previousRelativeWord() : -1;
    m_textString.erase(m_cursorString.getSize() + toRemove, -toRemove);
    moveCursor(toRemove);

    m_text.setString(m_textString);
    updateDynamicText();
}

void TextEntry::deleteNext()
{
    returnif (deleteSelection());
    returnif (m_cursorString == m_textString);

    int toRemove = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)? nextRelativeWord() : 1;
    m_textString.erase(m_cursorString.getSize(), toRemove);

    m_text.setString(m_textString);
    updateDynamicText();
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
    deleteSelection();

    returnif (m_textString.getSize() == m_maxCharacters);
    m_textString = m_cursorString + character + m_textString.substring(m_cursorString.getSize());
    moveCursor(1);
    deselect();

    m_text.setString(m_textString);
    updateDynamicText();
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

void TextEntry::refreshSelection()
{
    m_selectText.setString(m_selectString);
    m_selectRect.setSize({boundsSize(m_selectText).x, size().y - 2.f * m_textPadding});
    refreshSelectionPosition();
}

void TextEntry::refreshSelectionPosition()
{
    if (m_selectL2R) m_selectRect.setPosition(m_textPadding + m_selectShiftStart - m_textShift, m_textPadding);
    else m_selectRect.setPosition(m_textPadding - boundsSize(m_selectText).x + m_selectShiftStart - m_textShift, m_textPadding);
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
    m_text.setPosition(m_textPadding - m_textShift, m_textPadding);
    m_cursor.setPosition({m_textPadding - m_textShift + cursorTextSize.x + 0.1f * m_textPadding, 0.5f * m_textPadding});
    refreshSelectionPosition();
}

