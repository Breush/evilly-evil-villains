#include "dungeon/elements/trap.hpp"

#include "dungeon/inter.hpp"
#include "tools/random.hpp"

using namespace dungeon;
using namespace std::placeholders;

Trap::Trap(const RoomCoords& coords, ElementData& edata, Inter& inter)
    : baseClass(inter)
    , m_coords(coords)
{
    // TODO Should be in a bindElementData() function
    m_edata = &edata;

    const auto& trapID = m_edata->type();
    auto sTrapID = toString(trapID);

    // Initializing
    sf::Vector2f trapPosition = m_inter.positionFromRoomCoords(coords) + 0.5f * m_inter.tileSize();
    setLocalPosition(trapPosition);

    // Decorum
    m_sprite.load("vanilla/traps/" + sTrapID + "/anim");

    // Lua API
    m_lua["eev_warnHarvestableDosh"] = [this] { lua_warnHarvestableDosh(); };

    // Lua
    std::string luaFilename = "res/vanilla/traps/" + sTrapID + "/ai.lua";
    if (!m_lua.load(luaFilename))
        throw std::runtime_error("Failed to load Lua file: '" + luaFilename + "'. It might be a syntax error or a missing file.");
    m_lua["_register"]();

    m_lua["_reinit"]();
}

//--------------------//
//---- Resources -----//

uint32 Trap::harvestDosh()
{
    return m_lua["_harvestDosh"]();
}

uint32 Trap::harvestableDosh()
{
    return m_lua["_harvestableDosh"]();
}

//---------------------------//
//----- LUA interaction -----//

void Trap::lua_warnHarvestableDosh()
{
    m_inter.data().addEvent("harvestable_dosh_changed", m_coords);
}
