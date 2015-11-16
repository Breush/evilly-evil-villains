#pragma once

#include "context/event.hpp"
#include "tools/int.hpp"

namespace dungeon
{
    // Forward declarations

    class Monster;

    //! The position of a room.
    struct RoomCoords
    {
        uint x;
        uint y;
    };

    //! A dungeon event.
    struct Event : public context::Event
    {
        union
        {
            int delta;              //!< The difference between previous and current value of resources.
            RoomCoords room;        //!< The coordinates of a room whenever it is constructed/destroyed.
            const wchar_t* message; //!< An additional information.

            //! When a monster changed.
            struct
            {
                const wchar_t* id;  //!< The monster id.
            } monster;
        };
    };
}
