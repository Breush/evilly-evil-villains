#pragma once

#include "dungeon/databases/constraint.hpp"
#include "tools/int.hpp"

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

    //! A constraint limits the possibility of creating an element in certain position.

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
}
