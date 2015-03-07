#include "nui/textentry.hpp"

#include "core/application.hpp"
#include "resources/identifiers.hpp"

using namespace nui;

TextEntry::TextEntry()
{
    setFocusable(true);

    addPart(&m_background);
    m_background.setFillColor(sf::Color::White);

    addPart(&m_text);
    m_text.setFont(Application::context().fonts.get(FontID::NUI));
    m_text.setColor(sf::Color::Black);
    m_text.setCharacterSize(18);
}

void TextEntry::update()
{
    m_background.setSize(size());
    setPartClippingRect(&m_text, {0.f, 0.f, size().x, size().y});
}

void TextEntry::handleKeyboardEvent(const sf::Event& event)
{
    if (event.type == sf::Event::TextEntered) {
        // Enter
        returnif (event.text.unicode == 13);

        // Backspace
        if (event.text.unicode == 8) {
            returnif (m_string.isEmpty());
            m_string.erase(m_string.getSize() - 1);
        }
        else m_string += event.text.unicode;

        m_text.setString(m_string);
    }
}
