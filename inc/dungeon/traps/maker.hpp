#pragma once

#include "dungeon/data.hpp"
#include "dungeon/traps/pickpock.hpp"
#include "tools/platform-fixes.hpp" // make_unique
#include "tools/debug.hpp"
#include "tools/tools.hpp"

namespace dungeon
{
namespace traps
{
    //! Create and initialize the corresponding Trap from a ElementData.
    // TODO Make that on the same signature that facilities maker.
    inline std::unique_ptr<Trap> make(Data::Room& room)
    {
        returnif (!room.trap.exists()) nullptr;
        const auto& type = room.trap.type();

        if (type == L"pickpock") return std::make_unique<PickPock>(room.coords, room.trap);
        else throw std::runtime_error("Unknown ElementData type.");
    }

    //! Returns the cost for creating a trap.
    inline uint onCreateCost(const std::wstring& type)
    {
        if (type == L"pickpock") return 220u;
        else throw std::logic_error("Some trap has not been registered for onCreateDosh().");

        return 0u;
    }

    //! Returns the gain for destroying a trap.
    inline uint onDestroyGain(const ElementData& elementData)
    {
        returnif (!elementData.exists()) 0u;

        const auto& type = elementData.type();
        if (type == L"pickpock") return 40u + elementData.at(L"dosh").as_uint32();
        else throw std::logic_error("Some trap has not been registered for onDestroyGain().");

        return 0u;
    }
}
}

