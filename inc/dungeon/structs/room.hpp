#pragma once

#include "dungeon/structs/facility.hpp"
#include "dungeon/structs/roomflag.hpp"
#include "dungeon/structs/trap.hpp"

namespace dungeon
{
    //! Possible rooms state.
    enum class RoomState
    {
        UNKNOWN,        //!< Error state.
        EMPTY,          //!< Empty.
        CONSTRUCTED,    //!< Constructed.
    };

    //! A room has coordinates in the dungeon and a few elements (facilities/trap).
    struct Room
    {
        sf::Vector2u coords;                    //!< The floor/room coordinates of the room.
        RoomState state = RoomState::UNKNOWN;   //!< The current state.
        uint8 hide = RoomFlag::NONE;            //!< What parts of the room are hidden.

        // Elements
        std::vector<FacilityInfo> facilities;   //!< All the facilities.
        TrapInfo trap;                          //!< The trap protecting the room.
    };
}
