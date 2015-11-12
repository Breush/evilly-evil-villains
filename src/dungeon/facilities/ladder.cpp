#include "dungeon/facilities/ladder.hpp"

#include "dungeon/data.hpp"
#include "tools/tools.hpp"
#include "tools/string.hpp"

using namespace dungeon::facilities;
using namespace std::placeholders;

// FIXME
// Still have to find out how to force construction of "exit_end"

Ladder::Ladder(const sf::Vector2u& coords, FacilityInfo& facilityInfo, dungeon::Inter& inter)
    : baseClass(coords, facilityInfo.data, inter)
    , m_facilityInfo(facilityInfo)
    , m_lua(true)
{
    // Decorum
    attachChild(m_sprite);
    m_sprite.load("dungeon/facilities/ladder");
    m_sprite.centerOrigin();

    // Lua API
    std::function<void(const std::string&, const std::string&, const std::string&)> eev_addCallback = std::bind(&Ladder::lua_addCallback, this, _1, _2, _3);
    m_lua["eev_addCallback"] = eev_addCallback;
    m_lua["eev_setDepth"] = [this] (const lua_Number inDepth) { lua_setDepth(inDepth); };
    m_lua["eev_isLink"] = [this] { return lua_isLink(); };
    m_lua["eev_getRlink"] = [this] (const uint32 nth) { return lua_getRlink(nth); };
    m_lua["eev_addRlink"] = [this] (const uint32 direction) { lua_addRlink(direction); };
    m_lua["eev_selectAnimation"] = [this] (const std::string& animationKey) { lua_selectAnimation(animationKey); };
    m_lua["eev_log"] = [this] (const std::string& str) { lua_log(str); };

    // Load lua file
    std::wstring facilityID = L"ladder";
    std::string luaFilename = "res/ai/facilities/" + toString(facilityID) + ".lua";
    if (!m_lua.load(luaFilename))
        throw std::runtime_error("Failed to load Lua file: '" + luaFilename + "'. It might be a syntax error or a missing file.");
    m_lua["_register"]();

    m_lua["_reinit"]();
}

//---------------------------//
//----- LUA interaction -----//

void Ladder::lua_addCallback(const std::string& luaKey, const std::string& entityType, const std::string& condition)
{
    addDetectSignal(entityType, condition, [this, luaKey] (const uint32 UID) { m_lua[luaKey.c_str()](UID); });
}

void Ladder::lua_setDepth(const lua_Number inDepth)
{
    setDepth(static_cast<float>(inDepth));
}

bool Ladder::lua_isLink()
{
    return m_facilityInfo.isLink;
}

uint32 Ladder::lua_getRlink(const uint32 nth) const
{
    returnif (nth >= m_facilityInfo.rlinks.size()) 0u;
    return static_cast<uint32>(m_facilityInfo.rlinks.at(nth));
}

void Ladder::lua_addRlink(const uint32 direction)
{
    // TODO Should be passed through a dungeon data function
    // So that an event can occur and refresh the graph
    m_facilityInfo.rlinks.emplace_back(static_cast<Direction>(direction));
}

void Ladder::lua_selectAnimation(const std::string& animationKey)
{
    m_sprite.select(toWString(animationKey));
}

void Ladder::lua_log(const std::string& str) const
{
    // TODO Adjust
    std::cerr << "LUA [facility::ladder] " << str << std::endl;
}
