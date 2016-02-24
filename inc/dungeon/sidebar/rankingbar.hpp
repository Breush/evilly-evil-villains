#pragma once

#include "nui/entity.hpp"

#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

namespace dungeon
{
    //! A bar showing a ranking text and progression.

    class RankingBar final : public nui::Entity
    {
        using baseClass = scene::Entity;

    public:

        //! Constructor.
        RankingBar();

        //! Default destructor.
        ~RankingBar() = default;

        std::string _name() const final { return "dungeon::RankingBar"; }

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
        void setTextColor(const sf::Color& color);

        //! Set the color of the progress bar.
        void setProgressionColor(const sf::Color& color);

        //! Set the size of the progress bar.
        void setPercent(float percent);

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

        //! Refresh the shown percent of the progress bar.
        void refreshProgression();

        //! @}

    private:

        // Progression
        sf::RectangleShape m_progressionLeft;       //!< The left part of the progress bar.
        sf::RectangleShape m_progressionRight;      //!< The right part of the progress bar.
        sf::RectangleShape m_progressionMiddle;     //!< The middle part of the progress bar.
        sf::RectangleShape m_progressionFill;       //!< The filler of the progress bar.
        sf::Vector2u m_progressionFillTextureSize;  //!< The fill texture size.
        float m_progressionLeftWidth = 0.f;         //!< Width of the left part of the progress bar.
        float m_progressionRightWidth = 0.f;        //!< Width of the right part of the progress bar.
        float m_progressionWidth = 0.f;             //!< Width of the full progression.

        // Content
        sf::Text m_text;            //!< The text.
        sf::RectangleShape m_logo;  //!< The logo.

        // Control
        float m_width = 0.f;    //!< The width of the entity, as defined by the user.
        float m_percent = 0.f;  //!< How much the progress bar is filled.

        // Decorum
        float m_fontSize = 0.f; //!< The character size of texts.
        float m_height = 0.f;   //!< The height of the bar.
        float m_hPadding = 0.f; //!< The horizontal padding.
        float m_vPadding = 0.f; //!< The vertical padding.
        float m_logoSide = 0.f; //!< The size of the logo.
    };
}
