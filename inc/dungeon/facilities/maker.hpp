#pragma once

#include "dungeon/facilities/treasure.hpp"
#include "tools/platform-fixes.hpp" // make_unique
#include "tools/debug.hpp"
#include "tools/tools.hpp"

namespace dungeon
{
namespace facilities
{
    //! Create and initialize the corresponding Facility from a ElementData.
    inline std::unique_ptr<Facility> make(const sf::Vector2u& coords, ElementData& eData)
    {
        const auto& type = eData.type();
        if (type == L"treasure") return std::make_unique<Treasure>(coords, eData);
        // TODO Remove this quiet error when all facilities are on the same model
        // else throw std::runtime_error("Unknown ElementData type.");
        return nullptr;
    }

    //! Returns the cost for creating a facility.
    inline uint onCreateCost(const std::wstring& type)
    {
        if (type == L"treasure") return 0u;
        else if (type == L"ladder") return 10u;
        else if (type == L"entrance") return 15u;
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
        else throw std::logic_error("Some facility has not been registered for onDestroyGain().");

        return 0u;
    }
}
}

