#include "dungeon/elements/facility.hpp"

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
    m_lua["eev_hasTunnel"] = [this] { return lua_hasTunnel(); };
    m_lua["eev_addTunnel"] = [this] (const int32 x, const int32 y, bool relative) { lua_addTunnel(x, y, relative); };
    m_lua["eev_removeTunnels"] = [this] { lua_removeTunnels(); };

    // Load lua file
    std::string luaFilename = "res/ai/dungeon/facilities/" + sFacilityID + ".lua";
    if (!m_lua.load(luaFilename))
        throw std::runtime_error("Failed to load Lua file: '" + luaFilename + "'. It might be a syntax error or a missing file.");
    m_lua["_register"]();

    m_lua["_reinit"]();
}

//---------------------------//
//----- LUA interaction -----//

bool Facility::lua_hasSiblingFacility(const std::string& facilityID) const
{
    return m_inter.hasRoomFacility(m_coords, toWString(facilityID));
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

bool Facility::lua_hasTunnel() const
{
    return !m_facilityInfo.tunnels.empty();
}

void Facility::lua_addTunnel(const int32 x, const int32 y, bool relative)
{
    Tunnel tunnel;
    tunnel.coords.x = x;
    tunnel.coords.y = y;
    tunnel.relative = relative;
    m_facilityInfo.tunnels.emplace_back(std::move(tunnel));
}

void Facility::lua_removeTunnels()
{
    m_facilityInfo.tunnels.clear();
}