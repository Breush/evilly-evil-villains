#include "dungeon/facility.hpp"

#include "dungeon/data.hpp"
#include "dungeon/inter.hpp"
#include "tools/tools.hpp"
#include "tools/string.hpp"

using namespace dungeon;
using namespace std::placeholders;

Facility::Facility(const sf::Vector2u& coords, FacilityInfo& facilityInfo, dungeon::Inter& inter)
    : m_coords(coords)
    , m_inter(inter)
    , m_facilityInfo(facilityInfo)
    , m_lua(true)
{
    setDetectable(false);
    const auto& facilityID = facilityInfo.data.type();
    auto sFacilityID = toString(facilityID);

    // Initializing
    sf::Vector2f facilityPosition = m_inter.tileLocalPosition(coords) + 0.5f * m_inter.tileSize();
    setDetectRangeFactor(m_inter.tileSize().x);
    setLocalScale(m_inter.roomScale());
    setLocalPosition(facilityPosition);
    centerOrigin();

    // Decorum
    attachChild(m_sprite);
    m_sprite.load("dungeon/facilities/" + sFacilityID);

    // Lua API
    std::function<void(const std::string&, const std::string&, const std::string&)> eev_addCallback = std::bind(&Facility::lua_addCallback, this, _1, _2, _3);
    m_lua["eev_addCallback"] = eev_addCallback;
    m_lua["eev_setDepth"] = [this] (const lua_Number inDepth) { lua_setDepth(inDepth); };
    m_lua["eev_isLink"] = [this] { return lua_isLink(); };
    m_lua["eev_getRtunnel"] = [this] (const uint32 nth) { return lua_getRtunnel(nth); };
    m_lua["eev_addRtunnel"] = [this] (const uint32 direction) { lua_addRtunnel(direction); };
    m_lua["eev_selectAnimation"] = [this] (const std::string& animationKey) { lua_selectAnimation(animationKey); };
    m_lua["eev_hasSiblingFacility"] = [this] (const std::string& facilityID) { return lua_hasSiblingFacility(facilityID); };
    m_lua["eev_setVisible"] = [this] (bool isVisible) { lua_setVisible(isVisible); };
    m_lua["eev_hasLink"] = [this] { return lua_hasLink(); };
    m_lua["eev_getLinkRoomX"] = [this] { return lua_getLinkRoomX(); };
    m_lua["eev_getLinkRoomY"] = [this] { return lua_getLinkRoomY(); };
    m_lua["eev_getCurrentRoomX"] = [this] { return lua_getCurrentRoomX(); };
    m_lua["eev_getCurrentRoomY"] = [this] { return lua_getCurrentRoomY(); };
    m_lua["eev_log"] = [this] (const std::string& str) { lua_log(str); };

    // Load lua file
    std::string luaFilename = "res/ai/facilities/" + sFacilityID + ".lua";
    if (!m_lua.load(luaFilename))
        throw std::runtime_error("Failed to load Lua file: '" + luaFilename + "'. It might be a syntax error or a missing file.");
    m_lua["_register"]();

    m_lua["_reinit"]();
}

//---------------------------//
//----- LUA interaction -----//

void Facility::lua_addCallback(const std::string& luaKey, const std::string& entityType, const std::string& condition)
{
    addDetectSignal(entityType, condition, [this, luaKey] (const uint32 UID) { m_lua[luaKey.c_str()](UID); });
}

void Facility::lua_setDepth(const lua_Number inDepth)
{
    setDepth(static_cast<float>(inDepth));
}

bool Facility::lua_isLink()
{
    return m_facilityInfo.isLink;
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

void Facility::lua_selectAnimation(const std::string& animationKey)
{
    m_sprite.select(toWString(animationKey));
}

bool Facility::lua_hasSiblingFacility(const std::string& facilityID) const
{
    return m_inter.hasFacility(m_coords, toWString(facilityID));
}

void Facility::lua_setVisible(bool isVisible)
{
    setVisible(isVisible);
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

uint32 Facility::lua_getCurrentRoomX() const
{
    return m_coords.x;
}

uint32 Facility::lua_getCurrentRoomY() const
{
    return m_coords.y;
}

void Facility::lua_log(const std::string& str) const
{
    std::cerr << "LUA [facility::" << toString(m_facilityInfo.data.type()) << "] " << str << std::endl;
}
