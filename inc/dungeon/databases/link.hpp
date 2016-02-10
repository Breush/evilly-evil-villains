#pragma once

#include "dungeon/databases/constraint.hpp"

#include <SFML/System/Vector2.hpp>

#include <string>
#include <vector>

namespace dungeon
{
    //! Common data for links to a room.
    struct Link
    {
        uint id = -1u;                  //!< Reference to this link, for lua API.
        std::wstring originFacilityID;  //!< The facility that can create this link.
        bool unbreakable = false;       //!< Whether moving origin facility should destroy this link or not.

        std::wstring facilityID;    //!< A facility to create (can be empty for none).
        bool strong = false;        //!< If the origin facility is deleted, then the linked facility (if any) too.
        bool permissive = false;    //!< Whether the linked facility (if any) does not block construction.

        bool relink = false;            //!< Whether the linked facility (if any) has a link-back to the origin facility.
        uint relinkID = -1u;            //!< The link-back ID.
        bool relinkFacilityID = false;  //!< The facilityID to use if link-backing.
    };

    //! Fixed link to a room.
    struct FixedLink : public Link
    {
        bool relative = false;      //!< Whether the coordinates are specified in relative coordinates.
        sf::Vector2<int8> coords;   //!< The coordinates (relative or absolute).
    };

    //! Interactive link to a room, the program will require the player to indicate a room.
    struct InteractiveLink : public Link
    {
        std::vector<Constraint> constraints;    //!< Decides where the player can build.
    };
}
