#pragma once

#include "tools/int.hpp"
#include "tools/string.hpp"

namespace dungeon
{
    //! Defines the possibly accessible relative directions from a room.
    /*!
     *  They are defined this way to be quickly masked.
     *  So that (direction & 0xf - 1u) gives WEST -1 and EAST +1 (x axis)
     *  And (direction >> 0x4 - 1u) gives SOUTH -1 and NORTH +1 (y axis)
     */
    enum Direction : uint8
    {
        EAST =  0x12,
        WEST =  0x10,
        NORTH = 0x21,
        SOUTH = 0x01,
    };

    //! Convert a direction string into the enum.
    inline Direction directionFromString(const std::string& sDirection)
    {
        if (sDirection == "north")      return Direction::NORTH;
        else if (sDirection == "south") return Direction::SOUTH;
        else if (sDirection == "east")  return Direction::EAST;
        else if (sDirection == "west")  return Direction::WEST;
        else mquit("Unknown direction key " + sDirection);
    }

    //! Convert a direction wide string into the enum.
    inline Direction directionFromString(const std::wstring& sDirection)
    {
        if (sDirection == L"north")      return Direction::NORTH;
        else if (sDirection == L"south") return Direction::SOUTH;
        else if (sDirection == L"east")  return Direction::EAST;
        else if (sDirection == L"west")  return Direction::WEST;
        else wquit(L"Unknown direction key " + sDirection);
    }

    //! Returns the opposite direction.
    inline Direction oppositeDirection(Direction direction)
    {
        switch (direction) {
        case EAST:  return WEST;
        case WEST:  return EAST;
        case NORTH: return SOUTH;
        case SOUTH: return NORTH;
        default:    return NORTH;
        }
    }
}
