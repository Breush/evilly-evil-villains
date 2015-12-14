#pragma once

#include "tools/int.hpp"

namespace dungeon
{
    //! Define flags for selectiong some parts of a room.

    enum RoomFlag : uint8
    {
        NONE    = 0x0,
        FLOOR   = 0x1,
        WALL    = 0x2,
    };
}
