#pragma once

#include "tools/int.hpp"

namespace dungeon
{
    //! The traps.
    /*!
     *  A trap is an object/structure to act against an invader.
     *  There can be only one per dungeon room.
     */
    enum TrapID : uint8
    {
        NONE,       //!< No trap set.
        PICKPOCK,   //!< Steals a bit of money from an invader.
    };
}
