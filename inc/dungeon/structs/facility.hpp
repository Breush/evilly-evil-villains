#pragma once

#include "dungeon/elements/elementdata.hpp"
#include "dungeon/structs/tunnel.hpp"
#include "dungeon/databases/link.hpp"

namespace dungeon
{
    // Forward declarations.

    class FacilityData;

    //! A facility link.
    struct FacilityLink
    {
        const Link* common = nullptr;   //!< The base link from the database.
        sf::Vector2<uint8> coords;      //!< The final position of the link.
        bool relink = false;            //!< Are we a link created through an automated relink?
    };

    //! A facility information.
    struct FacilityInfo
    {
        const FacilityData* common = nullptr;           //!< All the common data.
        ElementData data;                               //!< The individual data.
        sf::Vector2<uint8> coords;                      //!< Room coordinates we're in.

        std::vector<Tunnel> tunnels;                    //!< All tunnels that allow a way to an other room.
        std::vector<FacilityLink> links;                //!< All the active links.

        bool stronglyLinked = false;                    //!< Has the facility been created by a strong link?
        bool barrier = false;                           //!< Is this block the room for passage?
        uint treasure = -1u;                            //!< The treasure held.
    };
}
