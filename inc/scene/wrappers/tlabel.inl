#include "core/application.hpp"

namespace scene
{
    template<class Text_t>
    TLabel<Text_t>::TLabel()
    {
        setDetectable(false);

        addPart(&m_text);
    }

    //-------------------//
    //----- Routine -----//

    template<class Text_t>
    void TLabel<Text_t>::updateSize()
    {
        const auto& bounds = m_text.getLocalBounds();
        setSize({bounds.left + bounds.width, bounds.top + bounds.height});
    }

    template<class Text_t>
    void TLabel<Text_t>::refreshNUI(const config::NUIGuides& cNUI)
    {
        baseClass::refreshNUI(cNUI);

        m_fontSize = cNUI.fontSize;

        if (m_prestyle != Prestyle::NONE) setPrestyle(m_prestyle);
        else setCharacterSize(cNUI.fontSize);
    }

    //------------------//
    //----- Visual -----//

    template<class Text_t>
    void TLabel<Text_t>::setFont(const std::string& fontID)
    {
        m_text.setFont(Application::context().fonts.get(fontID));
        updateSize();
    }

    template<class Text_t>
    void TLabel<Text_t>::setColor(const sf::Color& color)
    {
        m_text.setColor(color);
    }

    template<class Text_t>
    void TLabel<Text_t>::setCharacterSize(uint characterSize)
    {
        m_text.setCharacterSize(characterSize);
        updateSize();
    }

    template<class Text_t>
    void TLabel<Text_t>::setText(const std::wstring& text)
    {
        m_text.setString(text);
        updateSize();
    }

    template<class Text_t>
    void TLabel<Text_t>::setText(const std::wstring& text, const std::string& fontID)
    {
        m_text.setString(text);
        m_text.setFont(Application::context().fonts.get(fontID));
        updateSize();
    }

    template<class Text_t>
    void TLabel<Text_t>::setPrestyle(Prestyle prestyle)
    {
        m_prestyle = prestyle;

        switch (m_prestyle)
        {
            case Prestyle::NONE:
                break;

            case Prestyle::NUI:
                m_text.setFont(Application::context().fonts.get("nui"));
                m_text.setColor(sf::Color::White);
                m_text.setCharacterSize(m_fontSize);
                break;

            case Prestyle::NUI_SOBER:
                m_text.setFont(Application::context().fonts.get("mono"));
                m_text.setColor(sf::Color::Black);
                m_text.setCharacterSize(m_fontSize);
                break;

            case Prestyle::NUI_TITLE:
                m_text.setFont(Application::context().fonts.get("nui"));
                m_text.setColor(sf::Color::White);
                m_text.setStyle(sf::Text::Bold);
                m_text.setCharacterSize(m_fontSize);
                break;

            case Prestyle::MENU_TITLE:
                m_text.setFont(Application::context().fonts.get("horror"));
                m_text.setStyle(sf::Text::Style::Bold);
                m_text.setColor(sf::Color::White);
                m_text.setCharacterSize(1.6f * m_fontSize);
                break;

            case Prestyle::MENU_POPUP_TITLE:
                m_text.setFont(Application::context().fonts.get("horror"));
                m_text.setStyle(sf::Text::Bold);
                m_text.setColor(sf::Color::White);
                m_text.setCharacterSize(3.2f * m_fontSize);
                break;

            case Prestyle::MENU_SOBER:
                m_text.setFont(Application::context().fonts.get("mono"));
                m_text.setColor(sf::Color::White);
                m_text.setCharacterSize(1.1f * m_fontSize);
                break;

            case Prestyle::MENU_SOBER_LIGHT:
                m_text.setFont(Application::context().fonts.get("mono"));
                m_text.setColor({255u, 255u, 255u, 128u});
                m_text.setCharacterSize(m_fontSize);
                break;
        }

        updateSize();
    }
}
