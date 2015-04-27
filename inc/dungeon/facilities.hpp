#pragma once

#include "tools/int.hpp"

namespace dungeon
{
    //! The facilities.
    /*!
     *  A facility is an object/structure that helps an invader
     *  There can be only one per dungeon room.
     */
    enum FacilityID : uint8
    {
        ENTRANCE,   //!< The main entrance to the dungeon.
        LADDER,     //!< A ladder to climb upstairs.
        TREASURE,   //!< The main treasure.
        COUNT,      //!< Number of facilities. KEEP LAST.
    };
}

