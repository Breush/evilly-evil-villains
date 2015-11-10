#pragma once

#include "dungeon/facilities/treasure.hpp"
#include "dungeon/facilities/ladder.hpp"
#include "dungeon/facilities/entrance.hpp"
#include "dungeon/facilities/stairs.hpp"
#include "tools/platform-fixes.hpp" // make_unique
#include "tools/debug.hpp"
#include "tools/tools.hpp"

namespace dungeon
{
    // Forward declarations

    class Inter;

    // TODO Externalize some to FacilitiesDB

namespace facilities
{
    //! Create and initialize the corresponding Facility from a ElementData.
    inline std::unique_ptr<Facility> make(const sf::Vector2u& coords, ElementData& eData, dungeon::Inter& inter)
    {
        const auto& type = eData.type();
        if (type == L"treasure") return std::make_unique<Treasure>(coords, eData, inter);
        else if (type == L"ladder") return std::make_unique<Ladder>(coords, eData, inter);
        else if (type == L"entrance") return std::make_unique<Entrance>(coords, eData, inter);
        else if (type == L"stairs") return std::make_unique<Stairs>(coords, eData, inter);
        else throw std::runtime_error("Unknown ElementData type for Facility.");
    }

    //! Returns the cost for creating a facility.
    inline uint onCreateCost(const std::wstring& type)
    {
        if (type == L"treasure") return 5u;
        else if (type == L"ladder") return 10u;
        else if (type == L"entrance") return 15u;
        else if (type == L"stairs") return 90u;
        else throw std::logic_error("Some facility has not been registered for onCreateDosh().");

        return 0u;
    }

    //! Returns the gain for destroying a facility.
    inline uint onDestroyGain(const ElementData& elementData)
    {
        returnif (!elementData.exists()) 0u;

        const auto& type = elementData.type();
        if (type == L"treasure") return elementData.at(L"dosh").as_uint32();
        else if (type == L"ladder") return 2u;
        else if (type == L"entrance") return 3u;
        else if (type == L"stairs") return 10u;
        else throw std::logic_error("Some facility has not been registered for onDestroyGain().");

        return 0u;
    }
}
}

