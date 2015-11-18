#include "dungeon/element.hpp"

#include "dungeon/data.hpp"
#include "dungeon/inter.hpp"
#include "dungeon/detector.hpp"
#include "context/villains.hpp"
#include "tools/tools.hpp"
#include "tools/string.hpp"

// TODO This is cheating for eev_setUIDDataU32
#include "dungeon/hero.hpp"

using namespace dungeon;
using namespace std::placeholders;

Element::Element(dungeon::Inter& inter, bool isLerpable)
    : baseClass(isLerpable)
    , m_inter(inter)
    , m_lua(true)
{
    setDetectable(false);

    // Initializing
    setDetectRangeFactor(m_inter.tileSize().x);
    setLocalScale(m_inter.roomScale());

    // Decorum
    attachChild(m_sprite);

    // Lua API
    std::function<void(const std::string&, const std::string&, const std::string&)> eev_addCallback = std::bind(&Facility::lua_addCallback, this, _1, _2, _3);

    m_lua["eev_addCallback"] = eev_addCallback;
    m_lua["eev_setDepth"] = [this] (const lua_Number inDepth) { lua_setDepth(inDepth); };
    m_lua["eev_setVisible"] = [this] (bool isVisible) { lua_setVisible(isVisible); };

    m_lua["eev_setDataU32"] = [this] (const std::string& s, const uint32 value) { lua_setDataU32(s, value); };
    m_lua["eev_getDataU32"] = [this] (const std::string& s) { return lua_getDataU32(s); };
    m_lua["eev_initEmptyDataU32"] = [this] (const std::string& s, const uint32 value) { lua_initEmptyDataU32(s, value); };
    m_lua["eev_setDataFloat"] = [this] (const std::string& s, const lua_Number value) { lua_setDataFloat(s, value); };
    m_lua["eev_getDataFloat"] = [this] (const std::string& s) { return lua_getDataFloat(s); };
    m_lua["eev_initEmptyDataFloat"] = [this] (const std::string& s, const lua_Number value) { lua_initEmptyDataFloat(s, value); };

    m_lua["eev_setUIDDataU32"] = [this] (const uint32 UID, const std::string& s, const uint32 value) { lua_setUIDDataU32(UID, s, value); };
    m_lua["eev_getUIDDataU32"] = [this] (const uint32 UID, const std::string& s) { return lua_getUIDDataU32(UID, s); };

    m_lua["eev_selectAnimation"] = [this] (const std::string& animationKey) { lua_selectAnimation(animationKey); };
    m_lua["eev_isAnimationStopped"] = [this] { return lua_isAnimationStopped(); };
    m_lua["eev_restartAnimation"] = [this] { lua_restartAnimation(); };
    m_lua["eev_forwardAnimation"] = [this] (const lua_Number offset) { lua_forwardAnimation(offset); };

    m_lua["eev_borrowVillainDosh"] = [this] (const uint32 amount) { return lua_borrowVillainDosh(amount); };

    m_lua["eev_dungeonExplodeRoom"] = [this] (const uint x, const uint y) { lua_dungeonExplodeRoom(x, y); };

    m_lua["eev_log"] = [this] (const std::string& str) { lua_log(str); };
}

//---------------------------//
//----- LUA interaction -----//

void Element::lua_addCallback(const std::string& luaKey, const std::string& entityType, const std::string& condition)
{
    addDetectSignal(entityType, condition, [this, luaKey] (const uint32 UID) { m_lua[luaKey.c_str()](UID); });
}

void Element::lua_setDepth(const lua_Number inDepth)
{
    setDepth(static_cast<float>(inDepth));
}

void Element::lua_setVisible(bool isVisible)
{
    setVisible(isVisible);
}

//----- Element data

void Element::lua_setDataU32(const std::string& s, const uint32 value)
{
    auto ws = toWString(s);
    m_edata->operator[](ws).as_uint32() = value;
}

uint32 Element::lua_getDataU32(const std::string& s) const
{
    auto ws = toWString(s);
    return m_edata->operator[](ws).as_uint32();
}

void Element::lua_initEmptyDataU32(const std::string& s, const uint32 value)
{
    auto ws = toWString(s);
    if (!m_edata->exists(ws))
        m_edata->operator[](ws).init_uint32(value);
}

void Element::lua_setDataFloat(const std::string& s, const lua_Number value)
{
    auto ws = toWString(s);
    m_edata->operator[](ws).as_float() = static_cast<float>(value);
}

lua_Number Element::lua_getDataFloat(const std::string& s) const
{
    auto ws = toWString(s);
    return static_cast<lua_Number>(m_edata->operator[](ws).as_float());
}

void Element::lua_initEmptyDataFloat(const std::string& s, const lua_Number value)
{
    auto ws = toWString(s);
    if (!m_edata->exists(ws))
        m_edata->operator[](ws).init_float(static_cast<float>(value));
}

//----- Element data from UID

void Element::lua_setUIDDataU32(const uint32 UID, const std::string& s, const uint32 value)
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

uint32 Element::lua_getUIDDataU32(const uint32 UID, const std::string& s) const
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

//----- Animation

void Element::lua_selectAnimation(const std::string& animationKey)
{
    m_sprite.select(toWString(animationKey));
}

bool Element::lua_isAnimationStopped() const
{
    return !m_sprite.started();
}

void Element::lua_restartAnimation()
{
    m_sprite.restart();
}

void Element::lua_forwardAnimation(const lua_Number offset)
{
    m_sprite.forward(sf::seconds(static_cast<float>(offset)));
}

//----- Villain

uint32 Element::lua_borrowVillainDosh(const uint32 amount)
{
    returnif (!m_inter.villain().doshWallet.sub(amount)) 0u;
    return amount;
}

//----- Dungeon

void Element::lua_dungeonExplodeRoom(const uint x, const uint y)
{
    m_inter.destroyRoom(sf::Vector2u(x, y));
}

//----- Debug

void Element::lua_log(const std::string& str) const
{
    std::cerr << "LUA [" << detectKey() << "] " << str << std::endl;
}