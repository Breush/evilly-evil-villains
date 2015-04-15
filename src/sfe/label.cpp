#include "sfe/label.hpp"

#include "core/application.hpp"
#include "resources/identifiers.hpp"
#include "config/nui.hpp"

using namespace sfe;

Label::Label()
{
    addPart(&m_text);

    refreshDisplay();
}

void Label::updateSize()
{
    const auto& bounds = m_text.getLocalBounds();
    setSize({bounds.left + bounds.width, bounds.top + bounds.height});
}

void Label::refreshDisplay()
{
    config::NUI cNUI;

    m_text.setCharacterSize(cNUI.fontSize);

    updateSize();
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

void Label::setText(const std::wstring& text, FontID fontID)
{
    m_text.setString(text);
    m_text.setFont(Application::context().fonts.get(fontID));
    updateSize();
}

void Label::setPrestyle(PrestyleID prestyle)
{
    config::NUI cNUI;

    switch (prestyle)
    {
        case PrestyleID::MENU_TITLE:
            m_text.setFont(Application::context().fonts.get(FontID::HORROR));
            m_text.setStyle(sf::Text::Style::Bold);
            m_text.setColor(sf::Color::White);
            m_text.setCharacterSize(1.6f * cNUI.fontSize);
            updateSize();
            break;

        case PrestyleID::MENU_POPUP_TITLE:
            m_text.setFont(Application::context().fonts.get(FontID::HORROR));
            m_text.setStyle(sf::Text::Style::Bold);
            m_text.setColor(sf::Color::White);
            m_text.setCharacterSize(3.2f * cNUI.fontSize);
            updateSize();
            break;

        case PrestyleID::MENU_SOBER:
            m_text.setFont(Application::context().fonts.get(FontID::MONO));
            m_text.setColor(sf::Color::White);
            m_text.setCharacterSize(1.1f * cNUI.fontSize);
            updateSize();
            break;

        case PrestyleID::MENU_SOBER_LIGHT:
            m_text.setFont(Application::context().fonts.get(FontID::MONO));
            m_text.setColor({255u, 255u, 255u, 128u});
            m_text.setCharacterSize(1.f * cNUI.fontSize);
            updateSize();
            break;
    }
}
