#pragma once

#include "dungeon/elements/elementdata.hpp"

namespace dungeon
{
    // Forward declarations

    class MonsterData;

    //! More generic but dungeon-dependent info about monsters type.
    struct MonsterGeneric
    {
        const MonsterData* common = nullptr;    //!< All the common data.
        bool unlocked = false;                  //!< Is this monster type available?
        uint reserve = 0u;                      //!< How many monsters in the reserve?
        uint countdown = 0u;                    //!< How many seconds to wait before allowing hire again.
    };
}
