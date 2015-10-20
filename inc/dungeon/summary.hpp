#pragma once

#include "scene/entity.hpp"
#include "context/event.hpp"

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

    class Summary final : public scene::Entity, private context::EventReceiver
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

        //! Initialize after textures are loaded.
        void init();

        std::string _name() const final { return "dungeon::Summary"; }

        //---------------------//
        //! @name Dungeon data
        //! @{

        //! The data of the dungeon to be read from.
        void useData(Data& data);

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onSizeChanges() final;
        void refreshNUI(const config::NUIGuides& cNUI) final;

        //! @}

        //-----------------------//
        //! @name Dungeon events
        //! @{

        void receive(const context::Event& event) final;

        //! @}

        //--------------------------------//
        //! @name Internal change updates
        //! @{

        //! Set the perfect size for a correct display.
        void updateSize();

        //! To be called whenever the dungeon data changed.
        void refreshFromData();

        //! @}

    private:

        //! The data of the dungeon to be read from.
        Data* m_data;

        //! Dungeon name.
        sf::Text m_dungeonName;

        //! The bars containing elements.
        std::array<Bar, BAR_COUNT> m_bars;

        float m_fontSize = 0.f;     //!< The character size of texts.
        float m_barWidth = 0.f;     //!< The estimated width of an info bar.
        float m_barHeight = 0.f;    //!< The estimated height of an info bar.
        float m_hPadding = 0.f;     //!< The horizontal padding.
        float m_vPadding = 0.f;     //!< The vertical padding.
        float m_barImageSide = 0.f; //!< The size of the image in a bar.
    };
}
