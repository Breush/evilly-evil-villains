#pragma once

#include "scene/entity.hpp"
#include "tools/int.hpp"

#include <SFML/Graphics/Text.hpp>

namespace scene
{
    //! A wrapper around sf::Text-like as an entity.

    template<class Text_t>
    class TLabel : public Entity
    {
        using baseClass = Entity;

    public:

        //! Prestyle that contains font, size and color.
        enum class Prestyle
        {
            NONE,
            NUI,
            NUI_SOBER,
            NUI_TITLE,
            NUI_SMALL,
            NUI_SMALL_TITLE,
            MENU_TITLE,
            MENU_POPUP_TITLE,
            MENU_SOBER,
            MENU_SOBER_LIGHT,
        };

    public:

        //! Constructor.
        TLabel();

        //! Default destructor.
        ~TLabel() = default;

        std::string _name() const final { return "Label"; }

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

        //----------------//
        //! @name Getters
        //! @{

        //! Get the string.
        std::wstring getString() const { return m_text.getString().toWideString(); }

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void refreshNUI(const config::NUIGuides& cNUI) final;

        //! Updates the size of the entity to current bounds.
        void updateSize();

        //! @}

        //--------------------------//
        //! @name Inherited getters
        //! @{

        //! Get the text.
        Text_t& text() { return m_text; }

        //! @}

    private:

        // Style
        Prestyle m_prestyle = Prestyle::NONE;   //!< The font, color and size override.
        Text_t m_text;                          //!< The text.

        // Parameters
        float m_fontSize = 0.f; //!< Reference font size.
    };
}

#include "scene/wrappers/tlabel.inl"
