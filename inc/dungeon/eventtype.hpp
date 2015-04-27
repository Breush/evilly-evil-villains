#pragma once

namespace dungeon
{
    // Forward declarations

    enum FacilityID : uint8;

    //! All the possible dungeon events.
    enum class EventType
    {
        ROOM_DESTROYED,     //!< A room was destroyed.
        ROOM_CONSTRUCTED,   //!< A rom was constructed.
        DOSH_CHANGED,       //!< Dosh value changed, delta is set.
        FAME_CHANGED,       //!< Fame value changed, delta is set.
        MODE_CHANGED,       //!< Mode (design/invasion) changed.
        FACILITY_CHANGED,   //!< Room facility changed.
        ERROR,              //!< Whenever something cannot be done.
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

            //! When a facility changed.
            struct
            {
                FacilityID id;      //!< The facility that was modified.
                RoomCoords room;    //!< The coordinates of the room.
            } facility;

        };
    };
}
