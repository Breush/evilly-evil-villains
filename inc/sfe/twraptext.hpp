#pragma once

#include "tools/string.hpp"

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>

namespace sfe
{
    //! A sf::Text-like drawable that can wrap itself to fit a given width.
    //! This will add newline characters each time it's needed.

    template<class Text_t>
    class WrapText final : public sf::Drawable, public sf::Transformable
    {
    public:

        //! Default constructor.
        WrapText() = default;

        //! Default destructor.
        ~WrapText() = default;

        //----------------//
        //! @name Routine
        //! @{

        //! Implements sf::Drawable drawing routine.
        void draw(sf::RenderTarget& target, sf::RenderStates states) const final;

        //! @}

        //-----------------//
        //! @name Wrappers
        //! @{

        //! Set the string to wrap.
        void setString(const std::wstring& string);

        //! Get the source string.
        inline const sf::String& getString() const { return m_wrapString; };

        //! Set the default color.
        void setColor(const sf::Color& color);

        //! Get the default color.
        inline const sf::Color& getColor() const { return m_text.getColor(); }

        //! Set the default character size.
        void setCharacterSize(uint characterSize);

        //! Get the default character size.
        inline uint getCharacterSize() const { return m_text.getCharacterSize(); }

        //! Set the font.
        void setFont(const sf::Font& font);

        //! Get the font.
        inline const sf::Font* getFont() const { return m_text.getFont(); }

        //! Set text style.
        void setStyle(uint32 style);

        //! Get the font.
        inline uint32 getStyle() const { return m_text.getStyle(); }

        //! @}

        //---------------//
        //! @name Bounds
        //! @{

        //! Get the local bounding rectangle.
        inline sf::FloatRect getLocalBounds() const { return m_text.getLocalBounds(); }

        //! Get the global bounding rectangle.
        inline sf::FloatRect getGlobalBounds() const { return getTransform().transformRect(getLocalBounds()); }

        //! @}

        //----------------//
        //! @name Fitting
        //! @{

        //! Set the width within which the text should fit.
        void fitWidth(float inFitWidth);

        //! @}

    protected:

        //--------------------------------//
        //! @name Internal change updates
        //! @{

        //! Ask for a rewrapping of the current wrap string.
        void rewrap();

        //! @}

    private:

        Text_t m_text;              //!< The displayed text.
        std::wstring m_wrapString;  //!< The original string.
        float m_fitWidth = -1.f;    //!< The width to fit.
    };
}

#include "sfe/twraptext.inl"
