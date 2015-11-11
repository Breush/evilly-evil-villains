#include "dungeon/facilities/ladder.hpp"

#include "tools/tools.hpp"
#include "tools/string.hpp"

using namespace dungeon::facilities;
using namespace std::placeholders;

Ladder::Ladder(const sf::Vector2u& coords, ElementData& elementdata, dungeon::Inter& inter)
    : baseClass(coords, elementdata, inter)
{
    // Decorum
    attachChild(m_sprite);
    m_sprite.centerOrigin();

    // TODO Remove and replace with an animation file,
    // the lua file will decide which on to use
    setDesign(Design::MAIN);

    // Lua API
    std::function<void(const std::string&, const std::string&, const std::string&)> eev_addCallback = std::bind(&Ladder::lua_addCallback, this, _1, _2, _3);
    m_lua["eev_addCallback"] = eev_addCallback;
    m_lua["eev_setDepth"] = [this] (const lua_Number inDepth) { lua_setDepth(inDepth); };
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

void Ladder::lua_log(const std::string& str) const
{
    // TODO
    // std::cerr << "LUA [facility::" << toString(m_edata->type()) << "] " << str << std::endl;
}

//------------------//
//----- Design -----//

void Ladder::setDesign(Design design)
{
    switch (design) {
    case Design::MAIN:
        m_sprite.setTexture("dungeon/facilities/ladder/main");
        break;

    case Design::EXIT_MAIN:
        m_sprite.setTexture("dungeon/facilities/ladder/exit_main");
        break;

    case Design::EXIT_END:
        m_sprite.setTexture("dungeon/facilities/ladder/exit_end");
        break;
    }
}
