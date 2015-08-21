#include "sfe/label.hpp"

#include "core/application.hpp"

using namespace sfe;

Label::Label()
{
    setDetectable(false);

    addPart(&m_text);
}

//-------------------//
//----- Routine -----//

void Label::updateSize()
{
    const auto& bounds = m_text.getLocalBounds();
    setSize({bounds.left + bounds.width, bounds.top + bounds.height});
}

void Label::refreshNUI(const config::NUIGuides& cNUI)
{
    baseClass::refreshNUI(cNUI);

    m_fontSize = cNUI.fontSize;

    if (m_prestyle != Prestyle::NONE) setPrestyle(m_prestyle);
    else setCharacterSize(cNUI.fontSize);
}

//------------------//
//----- Visual -----//

void Label::setFont(const std::string& fontID)
{
    m_text.setFont(Application::context().fonts.get(fontID));
    updateSize();
}

void Label::setColor(const sf::Color& color)
{
    m_text.setColor(color);
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

void Label::setText(const std::wstring& text, const std::string& fontID)
{
    m_text.setString(text);
    m_text.setFont(Application::context().fonts.get(fontID));
    updateSize();
}

void Label::setPrestyle(Prestyle prestyle)
{
    m_prestyle = prestyle;

    switch (m_prestyle)
    {
        case Prestyle::NONE:
            break;

        case Prestyle::NUI:
            m_text.setFont(Application::context().fonts.get("res/font/nui.ttf"));
            m_text.setColor(sf::Color::White);
            m_text.setCharacterSize(m_fontSize);
            break;

        case Prestyle::NUI_SOBER:
            m_text.setFont(Application::context().fonts.get("res/font/mono.ttf"));
            m_text.setColor(sf::Color::Black);
            m_text.setCharacterSize(m_fontSize);
            break;

        case Prestyle::NUI_TITLE:
            m_text.setFont(Application::context().fonts.get("res/font/nui.ttf"));
            m_text.setColor(sf::Color::White);
            m_text.setStyle(sf::Text::Style::Bold);
            m_text.setCharacterSize(m_fontSize);
            break;

        case Prestyle::MENU_TITLE:
            m_text.setFont(Application::context().fonts.get("res/font/horror.ttf"));
            m_text.setStyle(sf::Text::Style::Bold);
            m_text.setColor(sf::Color::White);
            m_text.setCharacterSize(1.6f * m_fontSize);
            break;

        case Prestyle::MENU_POPUP_TITLE:
            m_text.setFont(Application::context().fonts.get("res/font/horror.ttf"));
            m_text.setStyle(sf::Text::Style::Bold);
            m_text.setColor(sf::Color::White);
            m_text.setCharacterSize(3.2f * m_fontSize);
            break;

        case Prestyle::MENU_SOBER:
            m_text.setFont(Application::context().fonts.get("res/font/mono.ttf"));
            m_text.setColor(sf::Color::White);
            m_text.setCharacterSize(1.1f * m_fontSize);
            break;

        case Prestyle::MENU_SOBER_LIGHT:
            m_text.setFont(Application::context().fonts.get("res/font/mono.ttf"));
            m_text.setColor({255u, 255u, 255u, 128u});
            m_text.setCharacterSize(m_fontSize);
            break;
    }

    updateSize();
}
