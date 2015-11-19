#include "dungeon/elements/hero.hpp"

#include "dungeon/heroesmanager.hpp"
#include "dungeon/inter.hpp"
#include "dungeon/data.hpp"
#include "tools/random.hpp"
#include "tools/tools.hpp"
#include "ai/node.hpp"

#include <stdexcept> // runtime_error

using namespace dungeon;

Hero::Hero(HeroesManager& manager, Inter& inter, Graph& graph)
    : baseClass("dungeon/heroes/", inter, graph)
    , m_manager(manager)
{
    // Lua API
    m_lua["eev_getOut"] = [this] { lua_getOut(); };
    m_lua["eev_stealTreasure"] = [this] { lua_stealTreasure(); };
}

//------------------------//
//----- Element data -----//

void Hero::rebindElementData()
{
}

//-------------------//
//----- Lua API -----//

void Hero::lua_getOut()
{
    returnif (!toNodeData(m_currentNode)->entrance);
    m_manager.heroGetsOut(this);
}

void Hero::lua_stealTreasure()
{
    auto maxStolenDosh = std::min(100u, toNodeData(m_currentNode)->treasure);
    auto stolenDosh = 1u + rand() % maxStolenDosh;
    m_manager.heroStealsTreasure(this, toNodeData(m_currentNode)->coords, stolenDosh);
}
