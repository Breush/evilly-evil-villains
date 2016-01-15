#pragma once

#include "nui/entity.hpp"

#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

namespace dungeon
{
    //! A summary bar showing a ressource value and logo.

    class SummaryBar final : public nui::Entity
    {
        using baseClass = scene::Entity;

    public:

        //! Constructor.
        SummaryBar();

        //! Default destructor.
        ~SummaryBar() = default;

        std::string _name() const final { return "dungeon::SummaryBar"; }

        //! Initialize after textures are loaded.
        void init();

        //----------------//
        //! @name Control
        //! @{

        //! Set the text's string.
        void setText(const std::wstring& text);

        //! Set the logo texture ID.
        void setLogo(const std::string& textureID);

        //! Set the color of the text.
        void setColor(const sf::Color& color);

        //! Set the width used.
        void setWidth(float width);

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onSizeChanges() final;
        void refreshNUI(const config::NUIGuides& cNUI) final;

        //! Update the absolute size.
        void updateSize();

        //! @}

        //------------//
        //! @name ICU
        //! @{

        //! Refresh the whole display.
        void refresh();

        //! @}

    private:

        // Background
        sf::RectangleShape m_backgroundLeft;    //!< The left part of the background.
        sf::RectangleShape m_backgroundRight;   //!< The right part of the background.
        sf::RectangleShape m_backgroundMiddle;  //!< The middle part of the background.
        float m_backgroundLeftWidth = 0.f;      //!< Width of the left part of the background.
        float m_backgroundRightWidth = 0.f;     //!< Width of the right part of the background.

        // Content
        sf::Text m_text;            //!< The text.
        sf::RectangleShape m_logo;  //!< The logo.

        // Control
        float m_width = 0.f;    //!< The width of the entity, as defined by the user.

        // Decorum
        float m_fontSize = 0.f; //!< The character size of texts.
        float m_height = 0.f;   //!< The height of the bar.
        float m_hPadding = 0.f; //!< The horizontal padding.
        float m_vPadding = 0.f; //!< The vertical padding.
        float m_logoSide = 0.f; //!< The size of the logo.
    };
}
