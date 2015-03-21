#pragma once

#include "scene/entity.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>

#include <array>

namespace dungeon
{
    // Forward declarations

    class Data;

    //! The different bars in the summary.
    enum SummaryBarID : uint8
    {
        BAR_DOSH,
        BAR_FAME,
        BAR_COUNT,  //!< Keep last.
    };

    //! A small window with main dungeon information.

    class Summary final : public scene::Entity
    {
        using baseClass = scene::Entity;

        //! The bar containing information of a resource.
        struct Bar
        {
            sf::RectangleShape logo;    //!< The logo beside text.
            sf::Text text;              //!< The text (usually numbers).
        };

    public:

        //! Constructor.
        Summary();

        //! Default destructor.
        ~Summary() = default;

        //---------------------//
        //! @name Dungeon data
        //! @{

        //! The data of the dungeon to be read from.
        void useData(const Data& data);

        //! @}

    protected:

        //--------------------------------//
        //! @name Internal change updates
        //! @{

        //! To be called whenever the dungeon data changed.
        void refreshFromData();

        //! @}

        //----------------//
        //! @name Routine
        //! @{

        void update() final;
        void refreshDisplay() final;

        //! Set the perfect size for a correct display.
        void updateSize();

        //! @}

    private:

        //! The data of the dungeon to be read from.
        const Data* m_data;

        //! A basic background.
        sf::RectangleShape m_background;

        //! Dungeon name.
        sf::Text m_dungeonName;

        //! The bars containing elements.
        std::array<Bar, BAR_COUNT> m_bars;

        float m_fontSize;       //!< The character size of texts.
        float m_barWidth;       //!< The estimated width of an info bar.
        float m_barHeight;      //!< The estimated height of an info bar.
        float m_hPadding;       //!< The horizontal padding.
        float m_vPadding;       //!< The vertical padding.
        float m_barImageSide;   //!< The size of the image in a bar.
    };
}