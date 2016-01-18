#pragma once

#include "nui/entity.hpp"

#include <SFML/Graphics/Text.hpp>

namespace nui
{
    //! Wrapper of sf::Text as a nui::Entity.
    //! This entity does not provide prestyles as a label.

    class Text final : public Entity
    {
        using baseClass = Entity;

    public:

        //! Constructor.
        Text();

        //! Default destructor.
        ~Text() = default;

        std::string _name() const final { return "nui::Text"; }

        //-----------------//
        //! @name Wrappers
        //! @{

        //! Set the source string.
        void setString(const sf::String& source);

        //! Get the source string.
        inline const sf::String& getString() const { return m_text.getString(); }

        //! Set the fill color.
        void setFillColor(const sf::Color& color);

        //! Get the fill color.
        inline const sf::Color& getFillColor() const { return m_text.getFillColor(); }

        //! Set the outline color.
        void setOutlineColor(const sf::Color& color);

        //! Get the outline color.
        inline const sf::Color& getOutlineColor() const { return m_text.getOutlineColor(); }

        //! Set the outline thickness.
        void setOutlineThickness(float thickness);

        //! Get the outline color.
        inline float getOutlineThickness() const { return m_text.getOutlineThickness(); }

        //! Set the default character size.
        void setCharacterSize(uint characterSize);

        //! Get the default character size.
        inline uint getCharacterSize() const { return m_text.getCharacterSize(); }

        //! Set the font.
        void setFont(const sf::Font& font);

        //! Set the font. Overload from fontID.
        void setFont(const std::string& fontID);

        //! Get the font.
        inline const sf::Font* getFont() const { return m_text.getFont(); }

        //! Set text style.
        void setStyle(uint32 style);

        //! Get the font.
        inline uint32 getStyle() const { return m_text.getStyle(); }

        //! @}

    protected:

        //-----------------//
        //! @name Wrappers
        //! @{

        //! Update absolute size.
        void updateSize();

        //! @}

    private:

        //! The text itself.
        sf::Text m_text;
    };
}
