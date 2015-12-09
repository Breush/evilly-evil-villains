#pragma once

#include "tools/int.hpp"

namespace dungeon
{
    //! Define flags for the locking state of an element.

    enum LockFlag : uint8
    {
        NONE    = 0x0,
        FLOOR   = 0x1,
        WALL    = 0x2,
    };

    using Lock = uint8;
}
