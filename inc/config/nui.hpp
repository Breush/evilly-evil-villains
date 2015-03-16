#pragma once

#include "tools/int.hpp"

#include <SFML/System/Vector2.hpp>

namespace config
{
    struct NUI
    {
        //! Initialize all the parameters, according to current display config file.
        NUI();

        float hPadding;     //!< Horizontal padding.
        float vPadding;     //!< Vertical padding.

        float borderThick;  //!< Thickness of borders.
    };
}
