#pragma once

#include "dungeon/data.hpp"
#include "dungeon/facilities/treasure.hpp"
#include "dungeon/facilities/ladder.hpp"
#include "dungeon/facilities/entrance.hpp"
#include "dungeon/facilities/stairs.hpp"
#include "tools/platform-fixes.hpp" // make_unique
#include "tools/debug.hpp"
#include "tools/tools.hpp"

namespace dungeon
{
namespace facilities
{
    //! Create and initialize the corresponding Facility from a ElementData.
    inline std::unique_ptr<Facility> make(const sf::Vector2u& coords, FacilityInfo& facilityInfo, dungeon::Inter& inter)
    {
        auto& eData = facilityInfo.data;
        const auto& type = eData.type();
        if (type == L"treasure") return std::make_unique<Treasure>(coords, eData, inter);
        else if (type == L"ladder") return std::make_unique<Ladder>(coords, facilityInfo, inter);
        else if (type == L"entrance") return std::make_unique<Entrance>(coords, eData, inter);
        else if (type == L"stairs") return std::make_unique<Stairs>(coords, eData, inter);
        else throw std::runtime_error("Unknown ElementData type for Facility.");
    }
}
}

