#include "nui/text.hpp"

#include "context/context.hpp"
#include "tools/string.hpp"

using namespace nui;

Text::Text()
{
    addPart(&m_text);
}

//-------------------//
//----- Routine -----//

void Text::updateSize()
{
    auto textSize = boundsSize(m_text);
    setSize(textSize);
}

//--------------------//
//----- Wrappers -----//

void Text::setString(const sf::String& source)
{
    m_text.setString(source);
    updateSize();
}

void Text::setFillColor(const sf::Color& color)
{
    m_text.setFillColor(color);
}

void Text::setOutlineColor(const sf::Color& color)
{
    m_text.setOutlineColor(color);
}

void Text::setOutlineThickness(float thickness)
{
    m_text.setOutlineThickness(thickness);
}

void Text::setCharacterSize(uint characterSize)
{
    m_text.setCharacterSize(characterSize);
    updateSize();
}

void Text::setFont(const sf::Font& font)
{
    m_text.setFont(font);
    updateSize();
}

void Text::setFont(const std::string& fontID)
{
    m_text.setFont(context::context.fonts.get(fontID));
    updateSize();
}

void Text::setStyle(uint32 style)
{
    m_text.setStyle(style);
    updateSize();
}
