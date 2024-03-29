#include "tools/tools.hpp"
#include "tools/int.hpp"

#include <sstream>

namespace sfe
{
    //-------------------//
    //----- Routine -----//

    template<class Text_t>
    void WrapText<Text_t>::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform *= getTransform();
        target.draw(m_text, states);
    }

    //--------------------//
    //----- Wrappers -----//

    template<class Text_t>
    void WrapText<Text_t>::setString(const sf::String& string)
    {
        m_wrapString = string.toWideString();
        rewrap();
    }

    template<class Text_t>
    void WrapText<Text_t>::setFillColor(const sf::Color& color)
    {
        m_text.setFillColor(color);
    }

    template<class Text_t>
    void WrapText<Text_t>::setOutlineColor(const sf::Color& color)
    {
        m_text.setOutlineColor(color);
    }

    template<class Text_t>
    void WrapText<Text_t>::setOutlineThickness(float thickness)
    {
        m_text.setOutlineThickness(thickness);
    }

    template<class Text_t>
    void WrapText<Text_t>::setCharacterSize(uint characterSize)
    {
        m_text.setCharacterSize(characterSize);
        rewrap();
    }

    template<class Text_t>
    void WrapText<Text_t>::setFont(const sf::Font& font)
    {
        m_text.setFont(font);
        rewrap();
    }

    template<class Text_t>
    void WrapText<Text_t>::setStyle(uint32 style)
    {
        m_text.setStyle(style);
        rewrap();
    }

    //-------------------//
    //----- Fitting -----//

    template<class Text_t>
    void WrapText<Text_t>::fitWidth(float inFitWidth)
    {
        m_fitWidth = inFitWidth;
        rewrap();
    }

    //-----------------------------------//
    //----- Internal changes update -----//

    template<class Text_t>
    void WrapText<Text_t>::rewrap()
    {
        returnif (m_fitWidth < 0.f);

        // Word wrap - greedy algorithm
        std::wstring prevString, string;
        uint i = 0u;

        while (i != m_wrapString.size()) {
            std::wstring word;
            wchar_t c;

            // Get word
            while (i < m_wrapString.size() && !iswspace(c = m_wrapString[i])) {
                word += c;
                ++i;
            }

            // Update string, and add a Return character
            // between last words if we went to far
            string += word;
            m_text.setString(string);

            if (boundsSize(m_text).x > m_fitWidth) {
                string = prevString + L'\n' + word;
                m_text.setString(string);
            }

            prevString = string;

            // Add separators
            while (i < m_wrapString.size() && iswspace(c = m_wrapString[i])) {
                string += c;
                ++i;
            }
        }
    }
}
