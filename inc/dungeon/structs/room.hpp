#pragma once

#include "dungeon/structs/facility.hpp"
#include "dungeon/structs/roomflag.hpp"
#include "dungeon/structs/trap.hpp"

namespace dungeon
{
    //! Room coords.
    //! Maximum size is therefore 256x256.
    using RoomCoords = sf::Vector2<uint8>;

    //! Room direction vector.
    using RoomDirection = sf::Vector2<int8>;

    //! Room relative coords.
    //! Converting to unsigned integer gives you the RoomCoords.
    using RoomRelCoords = sf::Vector2<float>;

    //! Room rectangle in relative coords.
    using RoomRelRect = sf::FloatRect;

    inline RoomCoords toCoords(const RoomRelCoords& relCoords) { return {static_cast<uint8>(relCoords.x), static_cast<uint8>(relCoords.y)}; }
    inline RoomRelCoords toRelCoords(const RoomCoords& coords) { return {static_cast<float>(coords.x), static_cast<float>(coords.y)}; }

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
        RoomCoords coords;                      //!< The floor/room coordinates of the room.
        RoomState state = RoomState::UNKNOWN;   //!< The current state.
        uint8 hide = RoomFlag::NONE;            //!< What parts of the room are hidden.

        // Elements
        std::vector<FacilityInfo> facilities;   //!< All the facilities.
        TrapInfo trap;                          //!< The trap protecting the room.
    };
}
