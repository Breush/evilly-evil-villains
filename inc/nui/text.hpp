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
