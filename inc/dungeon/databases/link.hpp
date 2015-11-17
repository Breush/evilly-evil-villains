#pragma once

#include "tools/int.hpp"

#include <string>
#include <vector>

namespace dungeon
{
    //! Decide how to interpret custom constraint.

    struct ConstraintParameter
    {
        enum class Type : uint8
        {
            NONE,
            EQUAL,
            // GREATER,
            // LESSER,
        };

        Type type = Type::NONE;
        int value = 0;
    };

    //! A constraint limits the possibility of creating an explicit link.

    struct Constraint
    {
        enum class Mode : uint8
        {
            UNKNOWN,    //!< Error state.
            INCLUDE,    //!< Include the specified.
            EXCLUDE,    //!< Exclude the specified.
        };

        Mode mode = Mode::UNKNOWN;
        ConstraintParameter x;
        ConstraintParameter y;
    };

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
