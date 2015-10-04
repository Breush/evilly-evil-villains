#pragma once

#include "context/event.hpp"
#include "tools/int.hpp"

// TODO Rename this file dungeon/event.hpp

namespace dungeon
{
    // Forward declarations

    class Hero;
    class Monster;

    //! The possible modes.
    enum class Mode
    {
        DESIGN,     //!< Design.
        INVASION,   //!< Invasion.
    };

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
            int delta;  //!< The difference between previous and current value of resources.
            Mode mode;  //!< The new mode.
            RoomCoords room;        //!< The coordinates of a room whenever it is constructed/destroyed.
            const wchar_t* message; //!< An additional information.

            //! When an action interacts with a room.
            struct
            {
                union
                {
                    Hero* hero;         //!< The hero concerned.
                    Monster* monster;   //!< The monster concerned.
                };
                RoomCoords room;    //!< The coordinates of the room.
            } action;

            //! When a facility changed.
            struct
            {
                RoomCoords room;    //!< The coordinates of the room.
            } facility;
        };
    };
}
