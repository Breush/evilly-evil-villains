#pragma once

#include "tools/int.hpp"

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/String.hpp>

#include <vector>
#include <map>

namespace sfe
{
    //! Extends sf::Text to be able to write dynamically colored and styled text.
    /*!
     *  One can use *text* to get bold text, _text_ to get underlined text,
     *  ~text~ to get italic text, #red or #ff0000 to get colored text.
     *  When the #colorKey is parsed, the following space is needed and discarded.
     *  Any character can be escaped with a preceeding backslash.
     */

    class RichText final : public sf::Drawable, public sf::Transformable
    {
        //! Parsed text info.
        struct TextInfo
        {
            sf::Text text;                      //!< The text indeed.
            sf::String string;                  //!< The string of the text.
            sf::String colorKey;                //!< Color key.
            uint32 style = sf::Text::Regular;   //!< Style (italic/bold/underlined).
            bool newLine = false;               //!< True if ends in new line.
        };

    public:

        //! Default constructor
        RichText() = default;

        //! Default destructor.
        ~RichText() = default;

        //--------------------------------//
        //! @name Compatibility functions
        //! @{

        //! Set the source string.
        void setString(const sf::String& source);

        //! Get the source string.
        inline const sf::String& getString() const { return m_sourceString; }

        //! Set the default color.
        void setColor(const sf::Color& color);

        //! Get the default color.
        inline const sf::Color& getColor() const { return m_defaultColor; }

        //! Set the default character size.
        void setCharacterSize(uint characterSize);

        //! Get the default character size.
        inline uint getCharacterSize() const { return m_characterSize; }

        //! Set the font.
        void setFont(const sf::Font& font);

        //! Get the font.
        inline const sf::Font* getFont() const { return m_font; }

        //! Set text style.
        void setStyle(uint32 style);

        //! Get the font.
        inline uint32 getStyle() const { return m_defaultStyle; }

        //! @}

        //---------------//
        //! @name Bounds
        //! @{

        //! Get the local bounding rectangle.
        inline sf::FloatRect getLocalBounds() const { return m_localBounds; }

        //! Get the global bounding rectangle.
        inline sf::FloatRect getGlobalBounds() const { return getTransform().transformRect(getLocalBounds()); }

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        //! Draw the entity.
        void draw(sf::RenderTarget& target, sf::RenderStates states) const final;

        //! @}

        //-------------------------//
        //! @name Chunk management
        //! @{

        //! Creates a new chunk to the texts info list if needed,
        //! copying textInfo parts and setting it to the new one.
        void createChunk(TextInfo*& pTextInfo);

        //! Returns the color from a color key (in plain text or hexadecimal).
        //! Color key should be in lower case.
        sf::Color interpretColor(const sf::String& colorKey) const;

        //! @}

        //--------------------------------//
        //! @name Internal change updates
        //! @{

        //! Refresh the font of all elements.
        void refreshFont();

        //! Refresh the color of all elements.
        void refreshColors();

        //! Refresh the style of all elements.
        void refreshStyle();

        //! Refresh the character size of all elements.
        void refreshCharacterSize();

        //! Refresh all texts from current source.
        void reparseSource();

        //! Refresh all texts positions.
        void refreshPositions();

        //! @}

    private:

        //! All the texts.
        std::vector<TextInfo> m_textsInfo;

        sf::String m_sourceString;      //!< The source string, with markup.
        sf::FloatRect m_localBounds;    //!< Local bounds.

        sf::Color m_defaultColor = sf::Color::White;    //!< The color to use when none is specified.
        uint32 m_defaultStyle = sf::Text::Regular;      //!< The style used when none is specified.
        uint m_characterSize = 0u;                      //!< Size of all texts.
        const sf::Font* m_font = nullptr;               //!< Font of all texts.
    };
}

