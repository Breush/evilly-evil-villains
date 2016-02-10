#pragma once

#include "dungeon/databases/constraint.hpp"
#include "tools/int.hpp"

#include <SFML/System/Vector2.hpp>

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

    //! Checks whether a vector of constraints excludes a certain coordinate or not.
    inline bool constraintsExclude(const std::vector<Constraint>& constraints, const sf::Vector2<uint8>& coords)
    {
        bool excluded = false;

        for (const auto& constraint : constraints) {
            // Are the coordinates concerned by this constraint?
            if (constraint.x.type == ConstraintParameter::Type::EQUAL)
                if (static_cast<uint8>(constraint.x.value) != coords.x) continue;
            if (constraint.y.type == ConstraintParameter::Type::EQUAL)
                if (static_cast<uint8>(constraint.y.value) != coords.y) continue;

            // If in include list, that's all right
            if (constraint.mode == Constraint::Mode::INCLUDE)
                return false;
            // If in exclude list, wait till the end (it might be in include list too)
            else if (constraint.mode == Constraint::Mode::EXCLUDE)
                excluded = true;
        }

        return excluded;
    }
}
