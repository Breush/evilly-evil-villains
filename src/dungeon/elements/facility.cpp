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
    lua()["eev_getCurrentRoomX"] = [this] { return lua_getCurrentRoomX(); };
    lua()["eev_getCurrentRoomY"] = [this] { return lua_getCurrentRoomY(); };
    lua()["eev_hasTreasure"] = [this] { return lua_hasTreasure(); };
    lua()["eev_setTreasure"] = [this] (const uint32 value) { lua_setTreasure(value); };
    lua()["eev_isLink"] = [this] { return lua_isLink(); };
    lua()["eev_hasLink"] = [this] { return lua_hasLink(); };
    lua()["eev_getLinkRoomX"] = [this] { return lua_getLinkRoomX(); };
    lua()["eev_getLinkRoomY"] = [this] { return lua_getLinkRoomY(); };
    lua()["eev_hasTunnel"] = [this] { return lua_hasTunnel(); };
    lua()["eev_addTunnel"] = [this] (const int32 x, const int32 y, bool relative) { lua_addTunnel(x, y, relative); };
    lua()["eev_hasBarrier"] = [this] { return lua_hasBarrier(); };
    lua()["eev_setBarrier"] = [this] (bool activated) { lua_setBarrier(activated); };
    lua()["eev_removeTunnels"] = [this] { lua_removeTunnels(); };

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
}

//------------------------//
//----- Element data -----//

void Facility::bindFacilityInfo(FacilityInfo& facilityInfo)
{
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
    return m_inter.hasRoomFacility(m_coords, toWString(facilityID));
}

uint32 Facility::lua_getSiblingFacility(const std::string& facilityID) const
{
    auto pFacility = m_inter.findRoomFacility(m_coords, toWString(facilityID));
    returnif (pFacility == nullptr) 0u;
    return pFacility->UID();
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
    return m_facilityInfo->treasure != -1u;
}

void Facility::lua_setTreasure(const uint32 value)
{
    m_inter.setRoomFacilityTreasure(m_coords, m_elementID, value);
}

bool Facility::lua_isLink()
{
    return m_facilityInfo->isLink;
}

bool Facility::lua_hasLink() const
{
    return (m_facilityInfo->link.x != -1u) && (m_facilityInfo->link.y != -1u);
}

uint32 Facility::lua_getLinkRoomX() const
{
    return m_facilityInfo->link.x;
}

uint32 Facility::lua_getLinkRoomY() const
{
    return m_facilityInfo->link.y;
}

bool Facility::lua_hasTunnel() const
{
    return !m_facilityInfo->tunnels.empty();
}

void Facility::lua_addTunnel(const int32 x, const int32 y, bool relative)
{
    m_inter.data().addFacilityTunnel(*m_facilityInfo, {x, y}, relative);
}

bool Facility::lua_hasBarrier() const
{
    return m_facilityInfo->barrier;
}

void Facility::lua_setBarrier(bool activated)
{
    m_inter.setRoomFacilityBarrier(m_coords, m_elementID, activated);
}

void Facility::lua_removeTunnels()
{
    m_facilityInfo->tunnels.clear();
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
