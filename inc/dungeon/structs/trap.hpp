#pragma once

#include "dungeon/elements/elementdata.hpp"

namespace dungeon
{
    // Forward declarations

    class TrapData;

    //! More generic but dungeon-dependent info about traps type.
    struct TrapGeneric
    {
        const TrapData* common = nullptr;   //!< All the common data.
        bool unlocked = false;              //!< Is this trap type available?
    };

    //! A individual trap in the dungeon.
    struct TrapInfo
    {
        ElementData data;   //!< Its data.
    };
}
