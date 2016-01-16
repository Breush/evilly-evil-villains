#include "nui/text.hpp"

#include "core/application.hpp"
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

void Text::setColor(const sf::Color& color)
{
    m_text.setColor(color);
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
    m_text.setFont(Application::context().fonts.get(fontID));
    updateSize();
}

void Text::setStyle(uint32 style)
{
    m_text.setStyle(style);
    updateSize();
}
