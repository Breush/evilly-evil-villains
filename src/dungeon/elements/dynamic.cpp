#include "dungeon/elements/dynamic.hpp"

#include "dungeon/data.hpp"
#include "dungeon/inter.hpp"

using namespace dungeon;

Dynamic::Dynamic(Inter& inter)
    : baseClass(inter)
{
    // Lua API
    lua()["eev_dynamicRemoveSelf"] = [this] { lua_dynamicRemoveSelf(); };
}

//------------------------//
//----- Element data -----//

void Dynamic::lua_dynamicRemoveSelf()
{
    m_inter.data().dynamicsManager().remove(this);
}

//------------------------//
//----- Element data -----//

void Dynamic::bindElementData(ElementData& edata)
{
    // It's our first time is previous data is not the same monster type
    bool firstTime = (m_edata == nullptr) || (m_elementID != edata.type());
    m_edata = &edata;

    // Get full data
    m_elementID = m_edata->type();
    auto sElementID = toString(m_elementID);

    // Initial position
    sf::Vector2f position = m_inter.positionFromRelCoords({m_edata->operator[](L"rx").as_float(), m_edata->operator[](L"ry").as_float()});
    setLocalPosition(position);

    // Reparameter from inter
    setDetectRangeFactor(m_inter.tileSize().x);

    // First time or new monsterID
    if (firstTime) {
        // Animated sprite
        m_sprite.load("vanilla/dynamics/" + sElementID + "/anim");

        // Lua
        std::string luaFilename = "res/vanilla/dynamics/" + sElementID + "/ai.lua";
        if (!lua().load(luaFilename))
            throw std::runtime_error("Failed to load Lua file: '" + luaFilename + "'. It might be a syntax error or a missing file.");

        // Clear all previous callbacks
        removeDetectSignals();
        m_leftClickAction.name = "";
        m_rightClickAction.name = "";
        m_leftClickAction.callback = nullptr;
        m_rightClickAction.callback = nullptr;

        lua()["_register"]();
    }

    // Finish update
    lua()["_reinit"]();
}
