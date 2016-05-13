#include "dungeon/elements/element.hpp"

#include "dungeon/data.hpp"
#include "dungeon/inter.hpp"
#include "dungeon/detector.hpp"
#include "scene/components/ai.hpp"
#include "context/villains.hpp"
#include "core/gettext.hpp"
#include "tools/tools.hpp"
#include "tools/string.hpp"

using namespace dungeon;
using namespace std::placeholders;

Element::Element(dungeon::Inter& inter)
    : m_inter(inter)
{
    // Components
    auto aiComponent = addComponent<scene::AI>(*this);
    m_lua = &aiComponent->luaState();

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
    std::function<void(const std::string&, const std::string&, const std::string&)> eev_callbackRegister = std::bind(&Element::lua_callbackRegister, this, _1, _2, _3);
    std::function<void(const std::string&, const std::string&)> eev_callbackClickLeftSet = std::bind(&Element::lua_callbackClickLeftSet, this, _1, _2);
    std::function<void(const std::string&, const std::string&)> eev_callbackClickRightSet = std::bind(&Element::lua_callbackClickRightSet, this, _1, _2);

    lua()["eev_callbackRegister"] = eev_callbackRegister;
    lua()["eev_callbackClickLeftSet"] = eev_callbackClickLeftSet;
    lua()["eev_callbackClickRightSet"] = eev_callbackClickRightSet;

    lua()["eev_roomClickedInteractive"] = [this] (const std::string& luaKey) { lua_roomClickedInteractive(luaKey); };

    lua()["eev_setDataBool"] = [this] (const std::string& s, const bool value) { return lua_setDataBool(s, value); };
    lua()["eev_getDataBool"] = [this] (const std::string& s) { return lua_getDataBool(s); };
    lua()["eev_initEmptyDataBool"] = [this] (const std::string& s, const bool value) { return lua_initEmptyDataBool(s, value); };
    lua()["eev_setDataU32"] = [this] (const std::string& s, const uint32 value) { return lua_setDataU32(s, value); };
    lua()["eev_getDataU32"] = [this] (const std::string& s) { return lua_getDataU32(s); };
    lua()["eev_addDataU32"] = [this] (const std::string& s, const uint32 value) { return lua_addDataU32(s, value); };
    lua()["eev_initEmptyDataU32"] = [this] (const std::string& s, const uint32 value) { return lua_initEmptyDataU32(s, value); };
    lua()["eev_setDataFloat"] = [this] (const std::string& s, const lua_Number value) { return lua_setDataFloat(s, value); };
    lua()["eev_getDataFloat"] = [this] (const std::string& s) { return lua_getDataFloat(s); };
    lua()["eev_addDataFloat"] = [this] (const std::string& s, const lua_Number value) { return lua_addDataFloat(s, value); };
    lua()["eev_initEmptyDataFloat"] = [this] (const std::string& s, const lua_Number value) { return lua_initEmptyDataFloat(s, value); };

    lua()["eev_setUIDDataU32"] = [this] (const uint32 UID, const std::string& s, const uint32 value) { lua_setUIDDataU32(UID, s, value); };
    lua()["eev_getUIDDataU32"] = [this] (const uint32 UID, const std::string& s) { return lua_getUIDDataU32(UID, s); };

    lua()["eev_selectAnimation"] = [this] (const std::string& animationKey) { lua_selectAnimation(animationKey); };
    lua()["eev_isAnimationStopped"] = [this] { return lua_isAnimationStopped(); };
    lua()["eev_restartAnimation"] = [this] { lua_restartAnimation(); };
    lua()["eev_forwardAnimation"] = [this] (const lua_Number offset) { lua_forwardAnimation(offset); };

    lua()["eev_soundPlay"] = [this] (const std::string& soundID) { lua_soundPlay(soundID); };

    lua()["eev_selectAnimationUID"] = [this] (const uint32 UID, const std::string& animationKey) { lua_selectAnimationUID(UID, animationKey); };

    lua()["eev_damageRange"] = [this] (const lua_Number rx, const lua_Number ry, const lua_Number relRange, const lua_Number basePower) { lua_damageRange(rx, ry, relRange, basePower); };
    lua()["eev_spawnDynamic"] = [this] (const std::string& shortDynamicID, const lua_Number rx, const lua_Number ry) { return lua_spawnDynamic(shortDynamicID, rx, ry); };
    lua()["eev_setDepth"] = [this] (const lua_Number inDepth) { lua_setDepth(inDepth); };
    lua()["eev_setVisible"] = [this] (bool isVisible) { lua_setVisible(isVisible); };

    lua()["eev_damageUID"] = [this] (const uint32 UID, const lua_Number amount) { lua_damageUID(UID, amount); };
    lua()["eev_setDepthUID"] = [this] (const uint32 UID, const lua_Number inDepth) { lua_setDepthUID(UID, inDepth); };
    lua()["eev_setVisibleUID"] = [this] (const uint32 UID, bool isVisible) { lua_setVisibleUID(UID, isVisible); };
    lua()["eev_setDetectVisibleUID"] = [this] (const uint32 UID, bool detectVisible) { lua_setDetectVisibleUID(UID, detectVisible); };
    lua()["eev_setDetectActiveUID"] = [this] (const uint32 UID, bool detectActive) { lua_setDetectActiveUID(UID, detectActive); };
    lua()["eev_resetClipAreasUID"] = [this] (const uint32 UID) { lua_resetClipAreasUID(UID); };
    lua()["eev_addClipAreaUID"] = [this] (const uint32 UID, const lua_Number rx, const lua_Number ry, const lua_Number rw, const lua_Number rh) { lua_addClipAreaUID(UID, rx, ry, rw, rh); };

    lua()["eev_borrowVillainDosh"] = [this] (const uint32 amount) { return lua_borrowVillainDosh(amount); };
    lua()["eev_giveDosh"] = [this] (const uint32 amount) { lua_giveDosh(amount); };
    lua()["eev_giveSoul"] = [this] (const uint32 amount) { lua_giveSoul(amount); };
    lua()["eev_giveFame"] = [this] (const uint32 amount) { lua_giveFame(amount); };

    lua()["eev_dungeonExplodeRoom"] = [this] (const uint x, const uint y) { lua_dungeonExplodeRoom(x, y); };
    lua()["eev_dungeonPushRoom"] = [this] (const uint x, const uint y, const std::string& direction, const uint animationDelay) { return lua_dungeonPushRoom(x, y, direction, animationDelay); };

    lua()["eev_log"] = [this] (const std::string& str) { lua_log(str); };
}

//-------------------//
//----- Routine -----//

void Element::updateRoutine(const sf::Time& dt)
{
    // Forward to lua
    lua()["_update"](dt.asSeconds());
}

bool Element::isPointOverable(const sf::Vector2f& relPos) const noexcept
{
    // Is the position over the hitbox of this element?
    auto position = relPos - getOrigin();
    return m_sprite.hitbox().contains(position);
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
    showMouseOverlay(relPos);
    return true;
}

void Element::handleMouseLeft()
{
    hideMouseOverlay();
}

//---------------------//
//----- Animation -----//

void Element::selectAnimation(const std::string& animationString)
{
    m_sprite.select(animationString);
}

//-------------------------//
//----- Mouse overlay -----//

void Element::showMouseOverlay(const sf::Vector2f& relPos)
{
    // FIXME MouseOverlay should change the cursor temporaly
    m_mouseOverlay.setLocalPosition(relPos + sf::Vector2f{0.f, -15.f});

    returnif (m_mouseOverlay.visible());
    m_sprite.setShader("core/nui/hover/hover");

    if (m_leftClickAction.callback != nullptr || m_rightClickAction.callback != nullptr)
        m_mouseOverlay.setVisible(true);
}

void Element::hideMouseOverlay()
{
    m_sprite.setShader("");
    returnif (!m_mouseOverlay.visible());
    m_mouseOverlay.setVisible(false);
}

//---------------------------//
//----- LUA interaction -----//

//----- Callbacks

void Element::lua_callbackRegister(const std::string& luaKey, const std::string& entityType, const std::string& condition)
{
    addDetectSignal(entityType, condition, [this, luaKey] (const uint32 UID) { lua()[luaKey.c_str()](UID); });
}

void Element::lua_callbackClickLeftSet(const std::string& luaKey, const std::string& actionName)
{
    m_leftClickAction.name = actionName;
    m_leftClickAction.callback = [this, luaKey] { lua()[luaKey.c_str()](); };
    m_mouseOverlay.setLeft(_(m_leftClickAction.name.c_str()));
}

void Element::lua_callbackClickRightSet(const std::string& luaKey, const std::string& actionName)
{
    m_rightClickAction.name = actionName;
    m_rightClickAction.callback = [this, luaKey] { lua()[luaKey.c_str()](); };
    m_mouseOverlay.setRight(_(m_rightClickAction.name.c_str()));
}

void Element::lua_roomClickedInteractive(const std::string& luaKey)
{
    // TODO If "this" is destroyed while waiting for the click,
    // Inter will emit this callback with a pending reference => crash
    // Two possible fixes:
    // - Do not send "this" but pass the facility coords and its facilityID as parameters for roomClickedInteractive,
    //   so that Inter will search for the facility once it needs it (and do nothing if it does not exists anymore).
    // - Have a procedure that asks Inter to remove the current callback when "this" is destroyed.
    //
    // The first solution is better, as it is more robust against facilities same room reloads.
    auto callback = [this, luaKey] (const RoomCoords& coords) { lua()[luaKey.c_str()](coords.x, coords.y); };
    m_inter.roomClickedInteractive(callback);
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
uint32 Element::lua_addDataU32(const std::string& s, const uint32 value)
{
    auto ws = toWString(s);
    return m_edata->operator[](ws).as_uint32() += value;
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

lua_Number Element::lua_addDataFloat(const std::string& s, const lua_Number value)
{
    auto ws = toWString(s);
    return m_edata->operator[](ws).as_float() += static_cast<float>(value);
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
    auto* entity = s_detector.find(UID);
    auto* element = dynamic_cast<Element*>(entity);
    returnif (element == nullptr);

    auto ws = toWString(s);
    element->edata()[ws].as_uint32() = value;
}

uint32 Element::lua_getUIDDataU32(const uint32 UID, const std::string& s) const
{
    const auto* entity = s_detector.find(UID);
    const auto* element = dynamic_cast<const Element*>(entity);
    returnif (element == nullptr) 0u;

    auto ws = toWString(s);
    return element->edata().at(ws).as_uint32();
}

//----- Animation

void Element::lua_selectAnimation(const std::string& animationKey)
{
    selectAnimation(animationKey);
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

//----- Animation from UID

void Element::lua_selectAnimationUID(const uint32 UID, const std::string& animationKey)
{
    auto* entity = s_detector.find(UID);
    auto* element = dynamic_cast<Element*>(entity);
    returnif (element == nullptr);

    element->selectAnimation(animationKey);
}

//----- Audio

void Element::lua_soundPlay(const std::string& soundID)
{
    context::context.sounds.play(soundID);
}

//----- Villain

uint32 Element::lua_borrowVillainDosh(const uint32 amount)
{
    returnif (!m_inter.villain().doshWallet.sub(amount)) 0u;
    return amount;
}

void Element::lua_giveDosh(const uint32 amount)
{
    m_inter.villain().doshWallet.add(amount);
}

void Element::lua_giveSoul(const uint32 amount)
{
    m_inter.data().soulWallet().add(amount);
}

void Element::lua_giveFame(const uint32 amount)
{
    m_inter.data().fameWallet().add(amount);
}

//----- Elements control

void Element::lua_damageRange(const lua_Number rx, const lua_Number ry, const lua_Number relRange, const lua_Number basePower)
{
    auto& data = m_inter.data();
    float range = detectRangeFactor() * relRange;

    auto hurtEntityFunc = [&data, range, basePower] (DetectEntity& entity, float distance) {
        float damageDealt = static_cast<float>(basePower) * (1.f - distance / range);

        // Damage monster
        auto monster = dynamic_cast<Monster*>(&entity);
        if (monster != nullptr) {
            data.monstersManager().damage(monster, damageDealt);
            return;
        }

        // Damage hero
        auto hero = dynamic_cast<Hero*>(&entity);
        if (hero != nullptr) {
            data.heroesManager().damage(hero, damageDealt);
            return;
        }
    };

    sf::Vector2f relPos = m_inter.positionFromRelCoords({static_cast<float>(rx), static_cast<float>(ry)});
    s_detector.applyInRange(relPos, range, hurtEntityFunc);
}

uint32 Element::lua_spawnDynamic(const std::string& dynamicID, const lua_Number rx, const lua_Number ry)
{
    auto id = toWString(dynamicID);
    sf::Vector2f relPos = m_inter.positionFromRelCoords({static_cast<float>(rx), static_cast<float>(ry)});
    return m_inter.spawnDynamic(relPos, id);
}

void Element::lua_setDepth(const lua_Number inDepth)
{
    setDepth(static_cast<float>(inDepth));
}

void Element::lua_setVisible(bool isVisible)
{
    setVisible(isVisible);
}

//----- Elements control from UID

void Element::lua_damageUID(const uint32 UID, const lua_Number amount)
{
    auto* entity = s_detector.find(UID);
    returnif (entity == nullptr);

    // TODO There's some design decision to made here...
    // Is this check against all types what we want?
    // This does not look very sustainable.
    // -> A solution might be a common interface for managers (all providing bool damage(Element*, float))
    //    and checking the downcast compatibility (returning false if not ok).
    //    But does this means we need Facilities/Traps Managers too?

    // Damage monster
    auto monster = dynamic_cast<Monster*>(entity);
    if (monster != nullptr) {
        m_inter.data().monstersManager().damage(monster, static_cast<float>(amount));
        return;
    }

    // Damage hero
    auto hero = dynamic_cast<Hero*>(entity);
    if (hero != nullptr) {
        m_inter.data().heroesManager().damage(hero, static_cast<float>(amount));
        return;
    }

    // TODO Traps/facilities durability
}

void Element::lua_setDepthUID(const uint32 UID, const lua_Number inDepth)
{
    auto* entity = s_detector.find(UID);
    returnif (entity == nullptr);

    entity->setDepth(static_cast<float>(inDepth));
}

void Element::lua_setVisibleUID(const uint32 UID, const bool isVisible)
{
    auto* entity = s_detector.find(UID);
    returnif (entity == nullptr);

    entity->setVisible(isVisible);
}

void Element::lua_setDetectVisibleUID(const uint32 UID, bool detectVisible)
{
    auto* entity = s_detector.find(UID);
    returnif (entity == nullptr);

    entity->setDetectVisible(detectVisible);
}

void Element::lua_setDetectActiveUID(const uint32 UID, bool detectActive)
{
    auto* entity = s_detector.find(UID);
    returnif (entity == nullptr);

    entity->setDetectActive(detectActive);
}

void Element::lua_resetClipAreasUID(const uint32 UID)
{
    auto* entity = s_detector.find(UID);
    returnif (entity == nullptr);

    entity->resetClipAreas();
}

void Element::lua_addClipAreaUID(const uint32 UID, const lua_Number rx, const lua_Number ry, const lua_Number rw, const lua_Number rh)
{
    auto* entity = s_detector.find(UID);
    returnif (entity == nullptr);

    sf::FloatRect clipArea(rx, ry, rw, rh);
    clipArea = m_inter.rectFromRelRect(clipArea);
    clipArea = m_inter.getTransform().transformRect(clipArea);
    entity->addClipArea(clipArea, true);
}

//----- Dungeon

void Element::lua_dungeonExplodeRoom(const uint x, const uint y)
{
    // Note: It's an explosion, we do not get any money back
    m_inter.destroyRoom({static_cast<uint8>(x), static_cast<uint8>(y)}, true);
}

bool Element::lua_dungeonPushRoom(const uint x, const uint y, const std::string& sDirection, const uint animationDelay)
{
    return m_inter.pushRoom(RoomCoords{static_cast<uint8>(x), static_cast<uint8>(y)}, directionFromString(sDirection), animationDelay);
}

//----- Debug

void Element::lua_log(const std::string& str) const
{
    std::cerr << "LUA [" << detectKey() << "] " << str << std::endl;
}
