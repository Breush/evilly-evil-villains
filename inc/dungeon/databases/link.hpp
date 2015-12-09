#pragma once

#include "dungeon/databases/constraint.hpp"

#include <string>
#include <vector>

namespace dungeon
{
    //! A link is an interactive way to say that a construction
    //! require more information. And can keep two entities informed.

    struct Link
    {
        enum class Style : uint8
        {
            UNKNOWN,    //!< Error state.
            IMPLICIT,   //!< Implicit, automatically created.
            EXPLICIT,   //!< Explicit, requires user decision.
        };

        Style style = Style::UNKNOWN;
        std::wstring id;                //!< The id to use when creating the second one.

        // Implicit-only
        int x = 0;
        int y = 0;

        // Explicit-only
        std::vector<Constraint> constraints;
    };
}
