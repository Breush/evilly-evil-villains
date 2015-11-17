#include "dungeon/facility.hpp"

#include "dungeon/data.hpp"
#include "dungeon/inter.hpp"
#include "tools/string.hpp"

using namespace dungeon;

Facility::Facility(const sf::Vector2u& coords, FacilityInfo& facilityInfo, dungeon::Inter& inter)
    : baseClass(inter)
    , m_coords(coords)
    , m_facilityInfo(facilityInfo)
{
    const auto& facilityID = facilityInfo.data.type();
    auto sFacilityID = toString(facilityID);

    // Initializing
    sf::Vector2f facilityPosition = m_inter.tileLocalPosition(coords) + 0.5f * m_inter.tileSize();
    setLocalPosition(facilityPosition);

    // Decorum
    m_sprite.load("dungeon/facilities/" + sFacilityID);

    // Lua API
    m_lua["eev_hasSiblingFacility"] = [this] (const std::string& facilityID) { return lua_hasSiblingFacility(facilityID); };
    m_lua["eev_getCurrentRoomX"] = [this] { return lua_getCurrentRoomX(); };
    m_lua["eev_getCurrentRoomY"] = [this] { return lua_getCurrentRoomY(); };
    m_lua["eev_hasTreasure"] = [this] { return lua_hasTreasure(); };
    m_lua["eev_setTreasure"] = [this] (const uint32 value) { lua_setTreasure(value); };
    m_lua["eev_isLink"] = [this] { return lua_isLink(); };
    m_lua["eev_hasLink"] = [this] { return lua_hasLink(); };
    m_lua["eev_getLinkRoomX"] = [this] { return lua_getLinkRoomX(); };
    m_lua["eev_getLinkRoomY"] = [this] { return lua_getLinkRoomY(); };
    m_lua["eev_getRtunnel"] = [this] (const uint32 nth) { return lua_getRtunnel(nth); };
    m_lua["eev_addRtunnel"] = [this] (const uint32 direction) { lua_addRtunnel(direction); };
    m_lua["eev_hasTunnel"] = [this] { return lua_hasTunnel(); };
    m_lua["eev_addTunnel"] = [this] (const uint32 x, const uint32 y) { lua_addTunnel(x, y); };
    m_lua["eev_removeTunnels"] = [this] { lua_removeTunnels(); };

    // Load lua file
    std::string luaFilename = "res/ai/facilities/" + sFacilityID + ".lua";
    if (!m_lua.load(luaFilename))
        throw std::runtime_error("Failed to load Lua file: '" + luaFilename + "'. It might be a syntax error or a missing file.");
    m_lua["_register"]();

    m_lua["_reinit"]();
}

//---------------------------//
//----- LUA interaction -----//

bool Facility::lua_hasSiblingFacility(const std::string& facilityID) const
{
    return m_inter.hasFacility(m_coords, toWString(facilityID));
}

uint32 Facility::lua_getCurrentRoomX() const
{
    return m_coords.x;
}

uint32 Facility::lua_getCurrentRoomY() const
{
    return m_coords.y;
}

bool Facility::lua_hasTreasure() const
{
    return m_facilityInfo.treasure != -1u;
}

void Facility::lua_setTreasure(const uint32 value)
{
    // TODO Same as addRtunnel
    m_facilityInfo.treasure = value;
}

bool Facility::lua_isLink()
{
    return m_facilityInfo.isLink;
}

bool Facility::lua_hasLink() const
{
    return (m_facilityInfo.link.x != -1u) && (m_facilityInfo.link.y != -1u);
}

uint32 Facility::lua_getLinkRoomX() const
{
    return m_facilityInfo.link.x;
}

uint32 Facility::lua_getLinkRoomY() const
{
    return m_facilityInfo.link.y;
}

uint32 Facility::lua_getRtunnel(const uint32 nth) const
{
    returnif (nth >= m_facilityInfo.rtunnels.size()) 0u;
    return static_cast<uint32>(m_facilityInfo.rtunnels.at(nth));
}

void Facility::lua_addRtunnel(const uint32 direction)
{
    // TODO Should be passed through a dungeon data function
    // So that an event can occur and refresh the graph
    m_facilityInfo.rtunnels.emplace_back(static_cast<Direction>(direction));
}

bool Facility::lua_hasTunnel() const
{
    return !m_facilityInfo.tunnels.empty();
}

void Facility::lua_addTunnel(const uint32 x, const uint32 y)
{
    // TODO Same as addRtunnel
    sf::Vector2u tunnelCoords(x, y);
    m_facilityInfo.tunnels.emplace_back(tunnelCoords);
}

void Facility::lua_removeTunnels()
{
    // TODO Same as addRtunnel
    m_facilityInfo.tunnels.clear();
}
