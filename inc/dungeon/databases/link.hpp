#pragma once

#include "tools/int.hpp"

#include <string>

namespace dungeon
{
    //! A link is an interactive way to say that a construction
    //! require more information. And can keep two entities informed.

    struct Link
    {
        enum class Style : uint8
        {
            UNKNOWN,
            IMPLICIT,
            EXPLICIT,
        };

        Style style = Style::UNKNOWN;
        std::wstring id;                //!< The id to use when creating the second one.

        // Implicit-only
        int x = 0;
        int y = 0;
    };
}
