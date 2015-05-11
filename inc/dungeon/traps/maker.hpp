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
    //! Create and initialize the corresponding Trap from a TrapData.
    inline std::unique_ptr<Trap> make(Data::Room& room)
    {
        returnif (!room.trap.exists()) nullptr;
        const auto& type = room.trap.type();

        if (type == L"pickpock") return std::make_unique<PickPock>(room.coords, room.trap);
        else throw std::runtime_error("Unknown TrapData type.");
    }
}
}

