#include "dungeon/trap.hpp"

#include "dungeon/inter.hpp"
#include "dungeon/detector.hpp"
#include "dungeon/hero.hpp"
#include "tools/random.hpp"

using namespace dungeon;
using namespace std::placeholders;

Trap::Trap(const sf::Vector2u& coords, ElementData& edata, Inter& inter)
    : m_inter(inter)
    , m_edata(edata)
    , m_coords(coords)
    , m_lua(true)
{
    setDetectable(false);
    const auto& trapID = edata.type();
    auto sTrapID = toString(trapID);

    // Initializing
    setDetectRangeFactor(m_inter.tileSize().x);

    // Position
    sf::Vector2f trapPosition = m_inter.tileLocalPosition(coords) + 0.5f * m_inter.tileSize();
    setLocalPosition(trapPosition);
    centerOrigin();

    // Decorum
    attachChild(m_sprite);
    m_sprite.load("dungeon/traps/" + sTrapID);
    m_sprite.setLocalScale(m_inter.roomScale());

    // Lua
    std::string luaFilename = "res/ai/traps/" + sTrapID + ".lua";
    if (!m_lua.load(luaFilename))
        throw std::runtime_error("Failed to load Lua file: '" + luaFilename + "'. It might be a syntax error or a missing file.");

    // Lua API
    std::function<void(const std::string&, const std::string&, const std::string&)> eev_addCallback = std::bind(&Trap::lua_addCallback, this, _1, _2, _3);
    m_lua["eev_addCallback"] = eev_addCallback;
    m_lua["eev_warnHarvestableDosh"] = [this] { lua_warnHarvestableDosh(); };
    m_lua["eev_setDataU32"] = [this] (const std::string& s, const uint32 value) { lua_setDataU32(s, value); };
    m_lua["eev_setUIDDataU32"] = [this] (const uint32 UID, const std::string& s, const uint32 value) { lua_setUIDDataU32(UID, s, value); };
    m_lua["eev_getDataU32"] = [this] (const std::string& s) { return lua_getDataU32(s); };
    m_lua["eev_getUIDDataU32"] = [this] (const uint32 UID, const std::string& s) { return lua_getUIDDataU32(UID, s); };
    m_lua["eev_initEmptyDataU32"] = [this] (const std::string& s, const uint32 value) { lua_initEmptyDataU32(s, value); };
    m_lua["eev_setAnimationLooping"] = [this] (const bool looping) { lua_setAnimationLooping(looping); };
    m_lua["eev_selectAnimation"] = [this] (const std::string& animationKey) { lua_selectAnimation(animationKey); };
    m_lua["eev_isAnimationStopped"] = [this] { return lua_isAnimationStopped(); };
    m_lua["eev_log"] = [this] (const std::string& str) { lua_log(str); };

    // Call for all things to register
    m_lua["_register"]();
}

//------------------//
//---- Routine -----//

void Trap::updateRoutine(const sf::Time& dt)
{
    // Forward to lua
    m_lua["_update"](dt.asSeconds());
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

void Trap::lua_addCallback(const std::string& luaKey, const std::string& entityType, const std::string& condition)
{
    addDetectSignal(entityType, condition, [this, luaKey] (const uint32 UID) { m_lua[luaKey.c_str()](UID); });
}

void Trap::lua_warnHarvestableDosh()
{
    Event devent;
    devent.type = "harvestable_dosh_changed";
    devent.room = {m_coords.x, m_coords.y};
    emitter()->emit(devent);
}

void Trap::lua_initEmptyDataU32(const std::string& s, const uint32 value)
{
    auto ws = toWString(s);
    if (!m_edata.exists(ws))
        m_edata[ws].init_uint32(value);
}

void Trap::lua_setDataU32(const std::string& s, const uint32 value)
{
    auto ws = toWString(s);
    m_edata[ws].as_uint32() = value;
}

void Trap::lua_setUIDDataU32(const uint32 UID, const std::string& s, const uint32 value)
{
    auto ws = toWString(s);
    auto* entity = s_detector.find(UID);
    returnif (entity == nullptr);

    if (entity->detectKey() == "hero") {
        auto* hero = reinterpret_cast<Hero*>(entity);
        // FIXME Cheating here... as hero have no edata yet
        hero->setDosh(value);
    }
}

uint32 Trap::lua_getDataU32(const std::string& s) const
{
    auto ws = toWString(s);
    return m_edata.at(ws).as_uint32();
}

uint32 Trap::lua_getUIDDataU32(const uint32 UID, const std::string& s) const
{
    auto ws = toWString(s);
    const auto* entity = s_detector.find(UID);
    returnif (entity == nullptr) 0u;

    if (entity->detectKey() == "hero") {
        const auto* hero = reinterpret_cast<const Hero*>(entity);
        // FIXME Cheating here... as hero have no edata yet
        return hero->dosh();
    }

    // Not found
    return 0u;
}

void Trap::lua_selectAnimation(const std::string& animationKey)
{
    m_sprite.select(toWString(animationKey));
}

void Trap::lua_setAnimationLooping(const bool looping)
{
    m_sprite.setLooping(looping);
}

bool Trap::lua_isAnimationStopped() const
{
    return !m_sprite.started();
}

void Trap::lua_log(const std::string& str) const
{
    std::cerr << "LUA [trap::" << toString(m_edata.type()) << "] " << str << std::endl;
}
