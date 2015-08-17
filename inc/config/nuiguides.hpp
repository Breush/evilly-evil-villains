#pragma once

#include "config/display.hpp"
#include "tools/int.hpp"

namespace config
{
    //! Guides for NUI elements' size.

    struct NUIGuides
    {
        //! Initialize all the parameters, according to current display config file.
        void recompute(const Display::NUI& cDisplayNUI);

        float hPadding;     //!< Horizontal padding.
        float vPadding;     //!< Vertical padding.

        float borderThick;  //!< Thickness of borders.

        float fontSize;     //!< Size of the font.
        float fontVSpace;   //!< The space to reserve for a correct display of a font at fontSize.
        float fontHSpace;   //!< The estimated space to reserve for a correct display of a font at fontSize.

        float hintSize;         //!< An hint for a proportional dimension.
        float hintImageSide;    //!< An hint for a proportional image side.
    };
}
