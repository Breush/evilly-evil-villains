#pragma once

#include "tools/int.hpp"

namespace config
{
    struct NUI
    {
        //! Initialize all the parameters, according to current display config file.
        NUI();

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
