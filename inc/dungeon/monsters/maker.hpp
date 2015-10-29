#pragma once

#include "dungeon/data.hpp"
#include "dungeon/monsters/creepim.hpp"
#include "tools/platform-fixes.hpp" // make_unique
#include "tools/debug.hpp"
#include "tools/tools.hpp"

namespace dungeon
{
    // Forward declarations

    class Inter;

namespace monsters
{
    //! Create and initialize the corresponding Monster from a ElementData.
    // FIXME This maker function should disappear once everything is externalized
    inline std::unique_ptr<Monster> make(sf::Vector2u& coords, ElementData& eData, dungeon::Inter& inter)
    {
        const auto& type = eData.type();
        if (type == L"creepim") return std::make_unique<Creepim>(coords, eData, inter);
        else mdebug_generic("ERROR: ", "Unknown ElementData type for Monster.");
        return nullptr;
    }

    //! Returns the gain for destroying a monster.
    inline uint onDestroyGain(const ElementData&)
    {
        // Well, why would you get money for destroying a monster?
        return 0u;
    }
}
}

