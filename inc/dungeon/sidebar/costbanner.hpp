#pragma once

#include "context/cost.hpp"
#include "nui/entity.hpp"
#include "sfe/richtext.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

#include <array>

namespace dungeon
{
    class CostBanner : public nui::Entity
    {
        using baseClass = nui::Entity;

        //! All icons.
        enum IconID
        {
            DOSH,
            SOUL,
            FAME,
            COUNT,  //!< Keep last.
        };

    public:

        //! Constructor.
        CostBanner();

        //! Default destructor.
        ~CostBanner() = default;

        std::string _name() const final { return "dungeon::CostBanner"; }

        //----------------//
        //! @name Control
        //! @{

        //! Adapt the display to the current cost.
        void setCost(const Cost& cost);

        //! Set a title for this banner.
        void setTitle(std::wstring title);

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void refreshNUI(const config::NUIGuides& cNUI) final;

        //! @}

        //--------------------------------//
        //! @name Internal changes update
        //! @{

        //! Refresh the elements.
        void refreshBanner();

        //! Pad a value to the string.
        void addString(std::wstring& str, bool& first, uint value, sf::RectangleShape& icon);

        //! @}

    private:

        std::wstring m_title;   //!< The banner's title.
        Cost m_cost;            //!< The cost to display.

        // Decorum
        sfe::RichText m_text;                                   //!< The text.
        float m_refSize = 0.f;                                  //!< The reference size.
        std::array<sf::RectangleShape, IconID::COUNT> m_icons;  //!< The icons.
    };
}
