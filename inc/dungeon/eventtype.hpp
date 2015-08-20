#pragma once

#include "tools/int.hpp"

namespace dungeon
{
    // Forward declarations

    class Hero;
    class Monster;

    //! All the possible dungeon events.
    enum class EventType
    {
        ROOM_DESTROYED,             //!< A room was destroyed.
        ROOM_CONSTRUCTED,           //!< A rom was constructed.
        DOSH_CHANGED,               //!< Dosh value changed, delta is set.
        FAME_CHANGED,               //!< Fame value changed, delta is set.
        MODE_CHANGED,               //!< Mode (design/invasion) changed.
        FACILITY_CHANGED,           //!< Room facility changed.
        TRAP_CHANGED,               //!< Room trap changed.
        MONSTER_ADDED,              //!< A monster has been added to the dungeon.
        MONSTER_EXPLODES_ROOM,      //!< A monster exploded in the dungeon.
        HARVESTABLE_DOSH_CHANGED,   //!< The harvestable dosh in a room changed.
        HERO_ENTERED_ROOM,          //!< A hero entered a room.
        HERO_LEFT_ROOM,             //!< A hero left a room.
        ERROR,                      //!< Whenever something cannot be done.
    };

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
    struct Event
    {
        EventType type; //!< The type of event which is sent.

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
