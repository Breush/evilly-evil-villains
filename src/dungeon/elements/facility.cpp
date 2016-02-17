#include "dungeon/elements/facility.hpp"

#include "dungeon/elements/movingelement.hpp"
#include "dungeon/detector.hpp"
#include "dungeon/data.hpp"
#include "dungeon/inter.hpp"
#include "scene/components/lightemitter.hpp"
#include "tools/string.hpp"

using namespace dungeon;

Facility::Facility(const RoomCoords& coords, ElementData& edata, dungeon::Inter& inter)
    : baseClass(inter)
    , m_coords(coords)
{
    m_edata = &edata;

    // Update from inter
    sf::Vector2f facilityPosition = m_inter.positionFromRoomCoords(coords) + 0.5f * m_inter.tileSize();
    setLocalPosition(facilityPosition);

    // Display reload
    m_elementID = m_edata->type();
    auto sElementID = toString(m_elementID);

    // FIXME We're all vanilla for now, but sElementID should probably hold the modID too
    m_sprite.load("vanilla/facilities/" + sElementID + "/anim");

    // Lua API
    lua()["eev_hasSiblingFacility"] = [this] (const std::string& facilityID) { return lua_hasSiblingFacility(facilityID); };
    lua()["eev_getSiblingFacility"] = [this] (const std::string& facilityID) { return lua_getSiblingFacility(facilityID); };

    lua()["eev_facilityExistsRelative"] = [this] (const int x, const int y, const std::string& facilityID) { return lua_facilityExistsRelative(x, y, facilityID); };

    lua()["eev_getCurrentRoomX"] = [this] { return lua_getCurrentRoomX(); };
    lua()["eev_getCurrentRoomY"] = [this] { return lua_getCurrentRoomY(); };
    lua()["eev_hasTreasure"] = [this] { return lua_hasTreasure(); };
    lua()["eev_setTreasure"] = [this] (const uint32 value) { lua_setTreasure(value); };

    lua()["eev_linkExists"] = [this] (uint32 linkID) { return lua_linkExists(linkID); };
    lua()["eev_linkGet"]    = [this] (uint32 linkID) { return lua_linkGet(linkID); };

    lua()["eev_energySendPulseRoom"] = [this] (const uint32 x, const uint32 y) { return lua_energySendPulseRoom(x, y); };

    lua()["eev_hasTunnel"] = [this] { return lua_hasTunnel(); };
    lua()["eev_addTunnel"] = [this] (const int32 x, const int32 y, bool relative) { lua_addTunnel(x, y, relative); };
    lua()["eev_removeTunnels"] = [this] { lua_removeTunnels(); };

    lua()["eev_hasBarrier"] = [this] { return lua_hasBarrier(); };
    lua()["eev_setBarrier"] = [this] (bool activated) { lua_setBarrier(activated); };

    // Room locks
    lua()["eev_roomLocksClear"] = [this] () { lua_roomLocksClear(); };
    lua()["eev_roomLocksAdd"] = [this] (const uint x, const uint y) { lua_roomLocksAdd(x, y); };

    // Lighting
    lua()["eev_lightAddPoint"] = [this] (lua_Number x, lua_Number y, lua_Number s) { return lua_lightAddPoint(x, y, s); };
    lua()["eev_lightSetColor"] = [this] (uint32 lightID, uint r, uint g, uint b) { return lua_lightSetColor(lightID, r, g, b); };

    // Load lua file
    std::string luaFilename = "res/vanilla/facilities/" + sElementID + "/ai.lua";
    if (!lua().load(luaFilename))
        throw std::runtime_error("Failed to load Lua file: '" + luaFilename + "'. It might be a syntax error or a missing file.");
    lua()["_register"]();
}

Facility::~Facility()
{
    // Might not exist
    removeComponent<scene::LightEmitter>();

    // Remove all our locks
    m_inter.facilityRoomLocksClear(m_coords, m_elementID);
}

//------------------------//
//----- Element data -----//

void Facility::bindFacilityInfo(FacilityInfo& facilityInfo)
{
    m_inter.facilityRoomLocksClear(m_coords, m_elementID);
    m_facilityInfo = &facilityInfo;

    // Lua update
    lua()["_reinit"]();
}

//-----------------------------//
//----- Elements callback -----//

void Facility::movingElementEnterTunnel(MovingElement& movingElement)
{
    lua()["_onEntityEnterTunnel"](movingElement.UID());
}

void Facility::movingElementLeaveTunnel(MovingElement& movingElement)
{
    lua()["_onEntityLeaveTunnel"](movingElement.UID());
}

//---------------------------//
//----- LUA interaction -----//

bool Facility::lua_hasSiblingFacility(const std::string& facilityID) const
{
    return m_inter.facilitiesExists(m_coords, toWString(facilityID));
}

uint32 Facility::lua_getSiblingFacility(const std::string& facilityID) const
{
    auto pFacility = m_inter.facilitiesFind(m_coords, toWString(facilityID));
    returnif (pFacility == nullptr) 0u;
    return pFacility->UID();
}

bool Facility::lua_facilityExistsRelative(const int x, const int y, const std::string& facilityID) const
{
    RoomCoords coords{static_cast<uint8>(m_coords.x + x), static_cast<uint8>(m_coords.y + y)};
    return m_inter.facilitiesExists(coords, toWString(facilityID));
}

uint32 Facility::lua_getCurrentRoomX() const
{
    return m_coords.x;
}

uint32 Facility::lua_getCurrentRoomY() const
{
    return m_coords.y;
}

//----- Treasure

bool Facility::lua_hasTreasure() const
{
    return m_facilityInfo->treasure != -1u;
}

void Facility::lua_setTreasure(const uint32 value)
{
    m_inter.setRoomFacilityTreasure(m_coords, m_elementID, value);
}

//----- Link

bool Facility::lua_linkExists(uint32 linkID) const
{
    for (const auto& link : m_facilityInfo->links)
        if (link.common != nullptr && link.common->id == linkID)
            return true;
    return false;
}

// TODO Update API text
std::tuple<uint32, uint32> Facility::lua_linkGet(uint32 linkID) const
{
    for (const auto& link : m_facilityInfo->links)
        if (link.common != nullptr && link.common->id == linkID)
            return std::tuple<uint32, uint32>{link.coords.x, link.coords.y};
    return std::tuple<uint32, uint32>{-1u, -1u};
}

//----- Signals

void Facility::lua_energySendPulseRoom(const uint32 x, const uint32 y)
{
    m_inter.energySendPulseRoom({static_cast<uint8>(x), static_cast<uint8>(y)});
}

//----- Tunnels

bool Facility::lua_hasTunnel() const
{
    return !m_facilityInfo->tunnels.empty();
}

void Facility::lua_addTunnel(const int32 x, const int32 y, bool relative)
{
    m_inter.data().addFacilityTunnel(*m_facilityInfo, {x, y}, relative);
}

void Facility::lua_removeTunnels()
{
    // TODO Let data do that properly...
    m_facilityInfo->tunnels.clear();
    m_inter.data().addEvent("dungeon_changed", m_coords);
}

//----- Barrier

bool Facility::lua_hasBarrier() const
{
    return m_facilityInfo->barrier;
}

void Facility::lua_setBarrier(bool activated)
{
    m_inter.setRoomFacilityBarrier(m_coords, m_elementID, activated);
}

//----- Room locks

void Facility::lua_roomLocksClear()
{
    m_inter.facilityRoomLocksClear(m_coords, m_elementID);
}

void Facility::lua_roomLocksAdd(const uint x, const uint y)
{
    RoomCoords lockingCoords{static_cast<uint8>(x), static_cast<uint8>(y)};
    m_inter.facilityRoomLocksAdd(m_coords, m_elementID, lockingCoords);
}

//----- Lighting

uint32 Facility::lua_lightAddPoint(lua_Number x, lua_Number y, lua_Number s)
{
    sf::Vector2f position{static_cast<float>(x), static_cast<float>(y)};
    float lightSize = static_cast<float>(s) * detectRangeFactor();

    position += getOrigin();

    // Become a light emitter if not yet
    auto pLightEmitter = getComponent<scene::LightEmitter>();
    if (pLightEmitter == nullptr)
        pLightEmitter = addComponent<scene::LightEmitter>(*this);
    return pLightEmitter->addPoint(position, lightSize);
}

void Facility::lua_lightSetColor(uint32 lightID, uint r, uint g, uint b)
{
    auto& lightEmitter = *getComponent<scene::LightEmitter>();
    sf::Color color(static_cast<uint8>(r), static_cast<uint8>(g), static_cast<uint8>(b));
    lightEmitter.setColor(lightID, std::move(color));
}
