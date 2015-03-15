#include "sfe/label.hpp"

#include "core/application.hpp"
#include "resources/identifiers.hpp"

using namespace sfe;

Label::Label()
{
    addPart(&m_text);
}

void Label::updateSize()
{
    const auto& bounds = m_text.getLocalBounds();
    setSize({bounds.left + bounds.width, bounds.top + bounds.height});
}

void Label::setFont(FontID fontID)
{
    m_text.setFont(Application::context().fonts.get(fontID));
    updateSize();
}

void Label::setCharacterSize(uint characterSize)
{
    m_text.setCharacterSize(characterSize);
    updateSize();
}

void Label::setText(const std::wstring& text)
{
    m_text.setString(text);
    updateSize();
}

void Label::setText(const std::wstring& text, FontID fontID, uint characterSize)
{
    m_text.setString(text);
    m_text.setFont(Application::context().fonts.get(fontID));
    m_text.setCharacterSize(characterSize);
    updateSize();
}

void Label::setPrestyle(PrestyleID prestyle)
{
    switch (prestyle)
    {
        case PrestyleID::MENU_TITLE:
            m_text.setFont(Application::context().fonts.get(FontID::HORROR));
            m_text.setStyle(sf::Text::Style::Bold);
            m_text.setColor(sf::Color::White);
            m_text.setCharacterSize(25);
            updateSize();
            break;

        case PrestyleID::MENU_POPUP_TITLE:
            m_text.setFont(Application::context().fonts.get(FontID::HORROR));
            m_text.setStyle(sf::Text::Style::Bold);
            m_text.setColor(sf::Color::White);
            m_text.setCharacterSize(50);
            updateSize();
            break;
    }
}
