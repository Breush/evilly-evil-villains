#pragma once

#include "dungeon/data.hpp"
#include "dungeon/monsters/creepim.hpp"
#include "tools/platform-fixes.hpp" // make_unique
#include "tools/debug.hpp"
#include "tools/tools.hpp"

namespace dungeon
{
namespace monsters
{
    //! Create and initialize the corresponding Monster from a ElementData.
    inline std::unique_ptr<Monster> make(const sf::Vector2u& coords, ElementData& eData)
    {
        const auto& type = eData.type();
        if (type == L"creepim") return std::make_unique<Creepim>(coords, eData);
        else throw std::runtime_error("Unknown ElementData type for Monster.");
    }
}
}

