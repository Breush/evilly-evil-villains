#pragma once

#include "nui/entity.hpp"
#include "nui/hstacker.hpp"
#include "nui/vstacker.hpp"
#include "nui/slider.hpp"
#include "nui/rectangleshape.hpp"

#include <array>

namespace nui
{
    //! A way to let the user easily choose a color.

    class ColorSelector final : public Entity
    {
        using baseClass = Entity;

    public:

        //! Constructor.
        ColorSelector();

        //! Default destructor.
        virtual ~ColorSelector() = default;

        virtual std::string _name() const override { return "nui::ColorSelector"; }

        //---------------//
        //! @name Status
        //! @{

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onChildSizeChanges(scene::Entity& child) final;

        //! Update the absolute size.
        void updateSize();

        //! @}

        //------------//
        //! @name ICU
        //! @{

        //! Update the rectangle color.
        void updateColorBox();

        //! @}

    private:

        // Content
        nui::HStacker m_globalStacker;          //!< Stacking everything else.
        nui::VStacker m_slidersStacker;         //!< Stacking the sliders.
        nui::RectangleShape m_colorBox;         //!< The currently selected color.
        std::array<nui::Slider, 3u> m_sliders;  //!< Red/Green/Blue sliders.
    };
}
