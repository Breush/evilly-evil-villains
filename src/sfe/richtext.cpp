#include "sfe/richtext.hpp"

#include "tools/string.hpp"
#include "tools/tools.hpp"

using namespace sfe;

//-------------------//
//----- Routine -----//

void RichText::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();

    for (auto& textInfo : m_textsInfo)
        target.draw(textInfo.text, states);
}

//-----------------------------------//
//----- Compatibility functions -----//

void RichText::setString(const sf::String& string)
{
    m_sourceString = string;
    reparseSource();
}

void RichText::setCharacterSize(uint characterSize)
{
    m_characterSize = characterSize;
    refreshCharacterSize();
}

void RichText::setFont(const sf::Font& font)
{
    m_font = &font;
    refreshFont();
}

void RichText::setFillColor(const sf::Color& color)
{
    m_fillColor = color;
    refreshColors();
}

void RichText::setOutlineColor(const sf::Color& color)
{
    m_outlineColor = color;
    refreshOutline();
}

void RichText::setOutlineThickness(float thickness)
{
    m_outlineThickness = thickness;
    refreshOutline();
}

void RichText::setStyle(uint32 style)
{
    m_defaultStyle = style;
    refreshStyle();
}

//-----------------------------------//
//----- Internal change updates -----//

void RichText::refreshFont()
{
    returnif (m_font == nullptr);

    for (auto& textInfo : m_textsInfo)
        textInfo.text.setFont(*m_font);
    refreshPositions();
}

void RichText::refreshColors()
{
    for (auto& textInfo : m_textsInfo)
        textInfo.text.setFillColor(interpretColor(textInfo.colorKey));
}

void RichText::refreshStyle()
{
    for (auto& textInfo : m_textsInfo)
        textInfo.text.setStyle(textInfo.style | m_defaultStyle);
}

void RichText::refreshCharacterSize()
{
    for (auto& textInfo : m_textsInfo)
        textInfo.text.setCharacterSize(m_characterSize);
    refreshPositions();
}

void RichText::refreshOutline()
{
    for (auto& textInfo : m_textsInfo) {
        textInfo.text.setOutlineColor(m_outlineColor);
        textInfo.text.setOutlineThickness(m_outlineThickness);
    }

    refreshPositions();
}

void RichText::reparseSource()
{
    // Clear
    m_textsInfo.clear();
    m_localBounds = {0.f, 0.f, 0.f, 0.f};

    // Create first
    m_textsInfo.emplace_back();
    auto pTextInfo = &m_textsInfo.back();

    // Parse whole string
    bool escaped = false;
    for (uint i = 0u; i < m_sourceString.getSize(); ++i) {
        // The character
        auto character = m_sourceString[i];

        // Add character if escaped
        if (escaped) {
            pTextInfo->string += character;
            escaped = false;
            continue;
        }

        // Detect special character
        switch (character) {
        // Italics
        case L'~':
            createChunk(pTextInfo);
            pTextInfo->style ^= sf::Text::Italic;
            break;

        // Bold
        case L'*':
            createChunk(pTextInfo);
            pTextInfo->style ^= sf::Text::Bold;
            break;

        // Underlined
        case L'_':
            createChunk(pTextInfo);
            pTextInfo->style ^= sf::Text::Underlined;
            break;

        // Color
        case L'#':
            createChunk(pTextInfo);
            pTextInfo->colorKey.clear();

            // Skip this character and search for first white space
            while (++i < m_sourceString.getSize() && !isspace(character = m_sourceString[i]))
                pTextInfo->colorKey += character;

            break;

        // Escape character
        case L'\\':
            escaped = true;
            break;

        // New line
        case L'\n':
            pTextInfo->newLine = true;
            createChunk(pTextInfo);
            break;

        // Normal character
        default:
            pTextInfo->string += character;
            break;
        }
    }

    // Affect string and all parameters to texts
    for (auto& textInfo : m_textsInfo) {
        textInfo.text.setString(textInfo.string);
        textInfo.text.setStyle(textInfo.style | m_defaultStyle);
        textInfo.text.setFillColor(interpretColor(textInfo.colorKey));
        textInfo.text.setOutlineColor(m_outlineColor);
        textInfo.text.setOutlineThickness(m_outlineThickness);
        textInfo.text.setCharacterSize(m_characterSize);
        if (m_font != nullptr) textInfo.text.setFont(*m_font);
    }

    refreshPositions();
}

void RichText::refreshPositions()
{
    returnif (m_font == nullptr);

    // Compute complete string
    sf::String completeString;
    for (const auto& textInfo : m_textsInfo) {
        completeString += textInfo.string;
        if (textInfo.newLine) completeString += L'\n';
    }

    // Generate the complete text
    sf::Text completeText;
    completeText.setString(completeString);
    completeText.setCharacterSize(m_characterSize);
    completeText.setFont(*m_font);

    // Save bounds
    m_localBounds = completeText.getLocalBounds();

    // Affect positions
    uint characterIndex = 0u;
    for (auto& textInfo : m_textsInfo) {
        textInfo.text.setPosition(completeText.findCharacterPos(characterIndex));
        characterIndex += textInfo.string.getSize();
        if (textInfo.newLine) ++characterIndex;
    }
}

//--------------------------------//
//----- Color interpretation -----//

void RichText::createChunk(TextInfo*& pTextInfo)
{
    // We need to create a new chunk if the previous one is not empty
    if (pTextInfo->newLine || pTextInfo->string.getSize() != 0u) {
        m_textsInfo.emplace_back();
        m_textsInfo.back().colorKey = pTextInfo->colorKey;
        m_textsInfo.back().style = pTextInfo->style;
        pTextInfo = &m_textsInfo.back();
    }
}

sf::Color RichText::interpretColor(const sf::String& colorKey) const
{
    returnif (colorKey.getSize() == 0u) m_fillColor;

    // Hexidecimal key (no color name starting with an F!)
    if (colorKey[0u] == L'0' || colorKey[0u] == L'f') {
        uint32 hexColorKey = 0x00ffffff;
        std::stringstream str(colorKey);
        str >> std::hex >> hexColorKey;
        return sf::Color((hexColorKey >> 16) & 0xff, (hexColorKey >> 8) & 0xff, (hexColorKey & 0xff), 0xff);
    }

    // Color name
    if      (colorKey == L"black")      return sf::Color::Black;
    else if (colorKey == L"blue")       return sf::Color::Blue;
    else if (colorKey == L"cyan")       return sf::Color::Cyan;
    else if (colorKey == L"green")      return sf::Color::Green;
    else if (colorKey == L"magenta")    return sf::Color::Magenta;
    else if (colorKey == L"red")        return sf::Color::Red;
    else if (colorKey == L"white")      return sf::Color::White;
    else if (colorKey == L"yellow")     return sf::Color::Yellow;

    return m_fillColor;
}
