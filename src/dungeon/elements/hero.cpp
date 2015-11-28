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
    m_lua["eev_stealTreasure"] = [this] { return lua_stealTreasure(); };
}

//---------------------//
//----- Loot info -----//

uint Hero::deadGain()
{
    return m_lua["_deadGain"]();
}

//------------------------//
//----- Element data -----//

void Hero::rebindElementData()
{
    const auto& heroData = m_inter.heroesDB().get(m_elementID);
    lerpable()->setPositionSpeed(m_inter.tileSize() * heroData.speed);
    m_pauseDelay = heroData.pauseDelay;
}

//-------------------//
//----- Lua API -----//

void Hero::lua_getOut()
{
    returnif (!toNodeData(m_currentNode)->entrance);
    m_manager.heroGetsOut(this);
}

uint Hero::lua_stealTreasure()
{
    auto maxStolenDosh = std::min(100u, toNodeData(m_currentNode)->treasure);
    auto stolenDosh = 1u + rand() % maxStolenDosh;
    return m_manager.heroStealsTreasure(this, toNodeData(m_currentNode)->coords, stolenDosh);
}
