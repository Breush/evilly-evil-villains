#include "nui/textentry.hpp"

#include "core/application.hpp"
#include "resources/identifiers.hpp"

using namespace nui;

// FIXME Add security to max string length

TextEntry::TextEntry()
    : m_textPadding(7.f)
{
    setFocusable(true);

    addPart(&m_background);
    m_background.setFillColor(sf::Color::White);

    addPart(&m_text);
    m_text.setFont(Application::context().fonts.get(FontID::NUI));
    m_text.setColor(sf::Color::Black);
    m_text.setCharacterSize(16); // TODO Get from config

    addPart(&m_cursor);
    m_cursor.setShade(0.f);
    m_cursor.setColor(sf::Color::Black);
    m_cursorText = m_text;
}

//-------------------//
//----- Routine -----//

void TextEntry::update()
{
    m_background.setSize(size());
    m_cursor.setLength(size().y - m_textPadding);

    setPartClippingRect(&m_text, {m_textPadding, 0.f, size().x - 2.f * m_textPadding, size().y});
    updateDynamicText();
}

void TextEntry::updateRoutine(const sf::Time& dt)
{
    // Blinking cursor
    m_cursorBlinkTime += dt.asSeconds();
    while (m_cursorBlinkTime >= 0.5f) {
        m_cursorBlinkTime -= 0.5f;
        if (m_cursorOn) removePart(&m_cursor);
        else addPart(&m_cursor);
        m_cursorOn = !m_cursorOn;
    }
}

//------------------//
//----- Events -----//

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
    }
    // Edit the text entry
    else if (event.type == sf::Event::TextEntered) {
        // Enter
        returnif (event.text.unicode == 13) false;

        // Backspace
        if (event.text.unicode == 8) {
            returnif (m_cursorString.isEmpty()) true;
            int toRemove = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)? previousRelativeWord() : -1;
            m_textString.erase(m_cursorString.getSize() + toRemove, -toRemove);
            moveCursor(toRemove);
        }
        // Delete
        else if (event.text.unicode == 127) {
            returnif (m_cursorString == m_textString) true;
            int toRemove = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)? nextRelativeWord() : 1;
            m_textString.erase(m_cursorString.getSize(), toRemove);
        }
        else {
            m_textString = m_cursorString + event.text.unicode + m_textString.substring(m_cursorString.getSize());
            moveCursor(1);
        }

        m_text.setString(m_textString);
        updateDynamicText();
        return true;
    }

    return false;
}

//----------------------//
//----- Management -----//

void TextEntry::moveCursor(int relativePos)
{
    returnif (relativePos == 0);
    returnif (relativePos < 0 && m_cursorString.isEmpty());
    returnif (relativePos > 0 && m_cursorString == m_textString);

    auto cursorStringSize = m_cursorString.getSize() + relativePos;
    m_cursorString = m_textString;

    m_cursorString.erase(cursorStringSize, m_textString.getSize() - cursorStringSize);
    m_cursorText.setString(m_cursorString);
    updateDynamicText();
}

void TextEntry::updateDynamicText()
{
    // Text
    m_text.setPosition(m_textPadding, m_textPadding);

    // Cursor
    const auto& bounds = m_cursorText.getLocalBounds();
    m_cursor.setPosition({m_textPadding + bounds.left + bounds.width + 1.f, 0.5f * m_textPadding});
}

int TextEntry::previousRelativeWord()
{
    int cursorStringSize = m_cursorString.getSize() - 1;
    for (int i = 0; i < cursorStringSize; ++i)
        returnif (!iswalnum(m_cursorString[cursorStringSize - i])) -(i + 1);
    return -(cursorStringSize + 1);
}

int TextEntry::nextRelativeWord()
{
    int cursorStringSize = m_cursorString.getSize();
    int textStringSize = m_textString.getSize() - 1;
    for (int i = 0; i <= textStringSize - cursorStringSize; ++i)
        returnif (!iswalnum(m_textString[cursorStringSize + i])) i + 1;
    return textStringSize - cursorStringSize + 1;
}

void TextEntry::setLength(float length)
{
    setSize({length + 2.f * m_textPadding, 16.f + 2.f * m_textPadding}); // 16.f = text_size
}