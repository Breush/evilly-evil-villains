#pragma once

#include "dungeon/elements/elementdata.hpp"
#include "dungeon/structs/tunnel.hpp"

namespace dungeon
{
    // Forward declarations.

    class FacilityData;

    //! A facility information.
    struct FacilityInfo
    {
        const FacilityData* common = nullptr;           //!< All the common data.
        ElementData data;                               //!< The individual data.
        std::vector<Tunnel> tunnels;                    //!< All tunnels that allow a way to an other room.
        sf::Vector2<uint8> link = {0xff_u8, 0xff_u8};   //!< If these info are absolutely linked to another room.
        bool isLink = false;                            //!< Are these info been created by a link?
        bool barrier = false;                           //!< Is this block the room for passage?
        uint treasure = -1u;                            //!< The treasure held.
    };
}
