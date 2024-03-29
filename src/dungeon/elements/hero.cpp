#include "dungeon/elements/hero.hpp"

#include "scene/components/lerpable.hpp"
#include "dungeon/managers/heroesmanager.hpp"
#include "dungeon/inter.hpp"
#include "dungeon/data.hpp"
#include "tools/random.hpp"
#include "tools/tools.hpp"
#include "ai/node.hpp"

#include <stdexcept> // runtime_error

using namespace dungeon;

Hero::Hero(HeroesManager& manager, Inter& inter, Graph& graph)
    : baseClass("vanilla/heroes/", inter, graph)
    , m_manager(manager)
{
    // Lua API
    lua()["eev_getOut"] = [this] { lua_getOut(); };
    lua()["eev_stealTreasure"] = [this] { return lua_stealTreasure(); };
}

//----------------------//
//----- Extern Lua -----//

void Hero::onDeath()
{
    lua()["_onDeath"]();
}

//-------------------//
//----- Control -----//

void Hero::setDamaged(bool damaged)
{
    // TODO we should probably select an animation
    // And do this for monster too
    m_sprite.setTiltColor(damaged? sf::Color::Red : sf::Color::White);
}

//------------------------//
//----- Element data -----//

void Hero::rebindElementData()
{
    const auto& heroData = m_inter.heroesDB().get(m_elementID);
    getComponent<scene::Lerpable>()->setPositionSpeed(m_inter.tileSize() * heroData.speed);
    m_pauseDelay = heroData.pauseDelay;
}

//-------------------//
//----- Lua API -----//

void Hero::lua_getOut()
{
    returnif (!m_currentNode->entrance);
    m_manager.heroGetsOut(this);
}

uint Hero::lua_stealTreasure()
{
    auto maxStolenDosh = std::min(100u, m_currentNode->treasure);
    auto stolenDosh = 1u + rand() % maxStolenDosh;
    return m_manager.heroStealsTreasure(this, m_currentNode->coords, stolenDosh);
}
