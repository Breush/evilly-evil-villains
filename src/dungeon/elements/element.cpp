#include "dungeon/elements/element.hpp"

#include "dungeon/data.hpp"
#include "dungeon/inter.hpp"
#include "dungeon/detector.hpp"
#include "context/villains.hpp"
#include "core/gettext.hpp"
#include "tools/tools.hpp"
#include "tools/string.hpp"

using namespace dungeon;
using namespace std::placeholders;

Element::Element(dungeon::Inter& inter, bool isLerpable)
    : baseClass(isLerpable)
    , m_inter(inter)
    , m_lua(true)
{
    // Note: an Element has a size fitting a whole room,
    // so the hitbox cannot be bigger than that.

    // Initializing
    // TODO Follow Monster design and delay that to a rebindFromData function
    setDetectRangeFactor(m_inter.tileSize().x);
    setLocalScale(m_inter.roomScale());
    setSize(m_inter.tileSize());
    centerOrigin();

    // Decorum
    attachChild(m_sprite);
    m_sprite.setRelativePosition({0.5f, 0.5f});

    // TODO Shouldn't it be done at NUI level?
    attachChild(m_mouseOverlay);
    m_mouseOverlay.setVisible(false);
    m_mouseOverlay.setDepth(-50.f);

    // Lua API
    std::function<void(const std::string&, const std::string&, const std::string&)> eev_addCallback = std::bind(&Element::lua_addCallback, this, _1, _2, _3);
    std::function<void(const std::string&, const std::string&)> eev_setLeftClickAction = std::bind(&Element::lua_setLeftClickAction, this, _1, _2);
    std::function<void(const std::string&, const std::string&)> eev_setRightClickAction = std::bind(&Element::lua_setRightClickAction, this, _1, _2);

    m_lua["eev_addCallback"] = eev_addCallback;
    m_lua["eev_setLeftClickAction"] = eev_setLeftClickAction;
    m_lua["eev_setRightClickAction"] = eev_setRightClickAction;

    m_lua["eev_setDepth"] = [this] (const lua_Number inDepth) { lua_setDepth(inDepth); };
    m_lua["eev_setVisible"] = [this] (bool isVisible) { lua_setVisible(isVisible); };

    m_lua["eev_setDataBool"] = [this] (const std::string& s, const bool value) { return lua_setDataBool(s, value); };
    m_lua["eev_getDataBool"] = [this] (const std::string& s) { return lua_getDataBool(s); };
    m_lua["eev_initEmptyDataBool"] = [this] (const std::string& s, const bool value) { return lua_initEmptyDataBool(s, value); };
    m_lua["eev_setDataU32"] = [this] (const std::string& s, const uint32 value) { return lua_setDataU32(s, value); };
    m_lua["eev_getDataU32"] = [this] (const std::string& s) { return lua_getDataU32(s); };
    m_lua["eev_initEmptyDataU32"] = [this] (const std::string& s, const uint32 value) { return lua_initEmptyDataU32(s, value); };
    m_lua["eev_setDataFloat"] = [this] (const std::string& s, const lua_Number value) { return lua_setDataFloat(s, value); };
    m_lua["eev_getDataFloat"] = [this] (const std::string& s) { return lua_getDataFloat(s); };
    m_lua["eev_initEmptyDataFloat"] = [this] (const std::string& s, const lua_Number value) { return lua_initEmptyDataFloat(s, value); };

    m_lua["eev_setUIDDataU32"] = [this] (const uint32 UID, const std::string& s, const uint32 value) { lua_setUIDDataU32(UID, s, value); };
    m_lua["eev_getUIDDataU32"] = [this] (const uint32 UID, const std::string& s) { return lua_getUIDDataU32(UID, s); };

    m_lua["eev_selectAnimation"] = [this] (const std::string& animationKey) { lua_selectAnimation(animationKey); };
    m_lua["eev_isAnimationStopped"] = [this] { return lua_isAnimationStopped(); };
    m_lua["eev_restartAnimation"] = [this] { lua_restartAnimation(); };
    m_lua["eev_forwardAnimation"] = [this] (const lua_Number offset) { lua_forwardAnimation(offset); };

    m_lua["eev_borrowVillainDosh"] = [this] (const uint32 amount) { return lua_borrowVillainDosh(amount); };

    m_lua["eev_dungeonExplodeRoom"] = [this] (const uint x, const uint y) { lua_dungeonExplodeRoom(x, y); };
    m_lua["eev_dungeonPushRoom"] = [this] (const uint x, const uint y, const std::string& direction, const uint animationDelay)
        { return lua_dungeonPushRoom(x, y, direction, animationDelay); };

    m_lua["eev_log"] = [this] (const std::string& str) { lua_log(str); };
}

//------------------//
//----- Events -----//

bool Element::handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& relPos, const sf::Vector2f&)
{
    // Is the mouse over the hitbox of this element?
    auto position = relPos - getOrigin();
    returnif (!m_sprite.hitbox().contains(position)) false;

    // Left mouse button pressed
    if (button == sf::Mouse::Left) {
        returnif (m_leftClickAction.callback == nullptr) false;
        m_leftClickAction.callback();
        return true;
    }
    // Right mouse button pressed
    else if (button == sf::Mouse::Right) {
        returnif (m_rightClickAction.callback == nullptr) false;
        m_rightClickAction.callback();
        return true;
    }

    return false;
}

bool Element::handleMouseMoved(const sf::Vector2f& relPos, const sf::Vector2f&)
{
    // Is the mouse over the hitbox of this element?
    auto position = relPos - getOrigin();
    if (!m_sprite.hitbox().contains(position)) {
        hideMouseOverlay();
        return false;
    }

    showMouseOverlay(relPos);
    return true;
}

void Element::handleMouseLeft()
{
    hideMouseOverlay();
}

//-------------------------//
//----- Mouse overlay -----//

void Element::showMouseOverlay(const sf::Vector2f& relPos)
{
    // FIXME MouseOverlay should change the cursor temporaly
    m_mouseOverlay.setLocalPosition(relPos + sf::Vector2f{0.f, -15.f});

    returnif (m_mouseOverlay.visible());

    // TODO Better use shaders when AnimatedSprite allows us
    m_sprite.setTiltColor({174u, 207u, 198u});

    if (m_leftClickAction.callback != nullptr || m_rightClickAction.callback != nullptr)
        m_mouseOverlay.setVisible(true);
}

void Element::hideMouseOverlay()
{
    m_sprite.setTiltColor(sf::Color::White);

    returnif (!m_mouseOverlay.visible());
    m_mouseOverlay.setVisible(false);
}

//---------------------------//
//----- LUA interaction -----//

void Element::lua_addCallback(const std::string& luaKey, const std::string& entityType, const std::string& condition)
{
    addDetectSignal(entityType, condition, [this, luaKey] (const uint32 UID) { m_lua[luaKey.c_str()](UID); });
}

void Element::lua_setLeftClickAction(const std::string& luaKey, const std::string& actionName)
{
    m_leftClickAction.name = actionName;
    m_leftClickAction.callback = [this, luaKey] { m_lua[luaKey.c_str()](); };
    m_mouseOverlay.setLeft(_(m_leftClickAction.name.c_str()));
}

void Element::lua_setRightClickAction(const std::string& luaKey, const std::string& actionName)
{
    m_rightClickAction.name = actionName;
    m_rightClickAction.callback = [this, luaKey] { m_lua[luaKey.c_str()](); };
    m_mouseOverlay.setRight(_(m_rightClickAction.name.c_str()));
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

bool Element::lua_setDataBool(const std::string& s, const bool value)
{
    auto ws = toWString(s);
    return m_edata->operator[](ws).as_bool() = value;
}

bool Element::lua_getDataBool(const std::string& s) const
{
    auto ws = toWString(s);
    return m_edata->operator[](ws).as_bool();
}

bool Element::lua_initEmptyDataBool(const std::string& s, const bool value)
{
    auto ws = toWString(s);
    if (!m_edata->exists(ws))
        m_edata->operator[](ws).init_bool(value);
    return m_edata->operator[](ws).as_bool();
}


uint32 Element::lua_setDataU32(const std::string& s, const uint32 value)
{
    auto ws = toWString(s);
    return m_edata->operator[](ws).as_uint32() = value;
}

uint32 Element::lua_getDataU32(const std::string& s) const
{
    auto ws = toWString(s);
    return m_edata->operator[](ws).as_uint32();
}

uint32 Element::lua_initEmptyDataU32(const std::string& s, const uint32 value)
{
    auto ws = toWString(s);
    if (!m_edata->exists(ws))
        m_edata->operator[](ws).init_uint32(value);
    return m_edata->operator[](ws).as_uint32();
}

lua_Number Element::lua_setDataFloat(const std::string& s, const lua_Number value)
{
    auto ws = toWString(s);
    return m_edata->operator[](ws).as_float() = static_cast<float>(value);
}

lua_Number Element::lua_getDataFloat(const std::string& s) const
{
    auto ws = toWString(s);
    return static_cast<lua_Number>(m_edata->operator[](ws).as_float());
}

lua_Number Element::lua_initEmptyDataFloat(const std::string& s, const lua_Number value)
{
    auto ws = toWString(s);
    if (!m_edata->exists(ws))
        m_edata->operator[](ws).init_float(static_cast<float>(value));
    return m_edata->operator[](ws).as_float();
}

//----- Element data from UID

void Element::lua_setUIDDataU32(const uint32 UID, const std::string& s, const uint32 value)
{
    auto ws = toWString(s);
    auto* entity = s_detector.find(UID);
    returnif (entity == nullptr);

    if (entity->detectKey() == "hero") {
        auto* hero = reinterpret_cast<Hero*>(entity);
        hero->edata()[ws].as_uint32() = value;
    }
}

uint32 Element::lua_getUIDDataU32(const uint32 UID, const std::string& s) const
{
    auto ws = toWString(s);
    const auto* entity = s_detector.find(UID);
    returnif (entity == nullptr) 0u;

    if (entity->detectKey() == "hero") {
        const auto* hero = reinterpret_cast<const Hero*>(entity);
        return hero->edata().at(ws).as_uint32();
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
    // Note: It's an explosion, we do not get any money back
    m_inter.destroyRoom(sf::Vector2u{x, y}, true);
}

bool Element::lua_dungeonPushRoom(const uint x, const uint y, const std::string& sDirection, const uint animationDelay)
{
    return m_inter.pushRoom(sf::Vector2u{x, y}, direction(sDirection), animationDelay);
}

//----- Debug

void Element::lua_log(const std::string& str) const
{
    std::cerr << "LUA [" << detectKey() << "] " << str << std::endl;
}
