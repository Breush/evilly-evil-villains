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
    inline std::unique_ptr<Monster> make(const sf::Vector2u& coords, ElementData& eData, dungeon::Inter& inter)
    {
        const auto& type = eData.type();
        if (type == L"creepim") return std::make_unique<Creepim>(coords, eData, inter);
        else throw std::runtime_error("Unknown ElementData type for Monster.");
    }

    //! Returns the cost for creating a monster.
    inline uint onCreateCost(const std::wstring& type)
    {
        if (type == L"creepim") return 550u;
        else throw std::logic_error("Some monster has not been registered for onCreateDosh().");

        return 0u;
    }

    //! Returns the gain for destroying a monster.
    inline uint onDestroyGain(const ElementData&)
    {
        // Well, why would you get money for destroying a monster?
        return 0u;
    }
}
}

