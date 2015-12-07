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
    };

    //! A monster, fully described.
    struct MonsterInfo
    {
        float hp = 0.f;         //!< Current health point of the monster.
        ElementData data;       //!< Type of monster, plus individual information.
    };
}
