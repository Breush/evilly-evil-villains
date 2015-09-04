#pragma once

#include "scene/entity.hpp"
#include "tools/int.hpp"

#include <SFML/Graphics/Text.hpp>

namespace sfe
{
    //! A wrapper around sf::Text as an entity.

    class Label final : public scene::Entity
    {
        using baseClass = scene::Entity;

    public:

        //! Prestyle that contains font, size and color.
        enum class Prestyle
        {
            NONE,
            NUI,
            NUI_SOBER,
            NUI_TITLE,
            MENU_TITLE,
            MENU_POPUP_TITLE,
            MENU_SOBER,
            MENU_SOBER_LIGHT,
        };

    public:

        //! Constructor.
        Label();

        //! Default destructor.
        ~Label() = default;

        //---------------//
        //! @name Visual
        //! @{

        //! Set text font.
        void setFont(const std::string& fontID);

        //! Set text color.
        void setColor(const sf::Color& color);

        //! Set character size.
        void setCharacterSize(uint characterSize);

        //! Set text string.
        void setText(const std::wstring& text);

        //! Set text string and font.
        void setText(const std::wstring& text, const std::string& fontID);

        //! Set a prestyle.
        void setPrestyle(Prestyle prestyle);

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void refreshNUI(const config::NUIGuides& cNUI) final;

        //! Updates the size of the entity to current bounds.
        void updateSize();

        //! @}

    private:

        // Style
        Prestyle m_prestyle = Prestyle::NONE;   //!< The font, color and size override.
        sf::Text m_text;                        //!< The text.

        // Parameters
        float m_fontSize;   //!< Reference font size.
    };
}
