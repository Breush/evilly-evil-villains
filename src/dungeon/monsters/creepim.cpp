#include "dungeon/monsters/creepim.hpp"

#include "dungeon/inter.hpp"

using namespace dungeon::monsters;
using namespace std::placeholders;

// TODO Remove coords and use only rx/ry

Creepim::Creepim(sf::Vector2u& coords, ElementData& elementdata, dungeon::Inter& inter)
    : baseClass(coords, elementdata, inter)
{
    lerpable()->setPositionSpeed({50.f, 25.f});

    // Initial position
    if (m_elementdata.empty()) {
        m_elementdata[L"rx"].init_float(0.f);
        m_elementdata[L"ry"].init_float(0.f);
    }

    const auto tileLocalPosition = m_inter.tileLocalPosition(m_coords);
    const auto monsterTilePosition = sf::Vector2f{m_inter.tileSize().x / 2.f, m_inter.tileSize().y * 0.62f};
    lerpable()->setTargetPosition(tileLocalPosition + monsterTilePosition);
    setLocalPosition(lerpable()->targetPosition());

    // Decorum
    attachChild(m_sprite);
    m_sprite.load("dungeon/monsters/creepim");
    m_sprite.setStarted(false);
    m_sprite.setLocalScale(m_inter.roomScale());

    // Lua
    m_luaActor.loadFile("res/ai/monsters/creepim.lua");
    // All moving elements inside dungeon should become dungeon::DetectEntity
    // Register through the dungeon detecter (function inherited from dungeon::DetectEntity)
    /* std::function<void(const std::string&, const std::string&)> eev_addCallback = std::bind(&DetectEntity::addCallback, this, _1, _2);
    m_luaActor.state()["eev_addCallback"] = eev_addCallback;
    m_luaActor.state()["register"]();*/

    refreshFromActivity();
}

//------------------//
//---- Routine -----//

void Creepim::onTransformChanges()
{
    m_elementdata[L"rx"].as_float() = localPosition().x / m_inter.size().x;
    m_elementdata[L"ry"].as_float() = localPosition().y / m_inter.size().y;
}

void Creepim::updateAI(const sf::Time& dt)
{
    returnif (m_fusing);
    returnif (!active());
    returnif (lerpable()->positionLerping());

    // Get next room if not already moving
    setCurrentNode(m_luaActor.findNextNode(m_currentNode));
}

void Creepim::updateRoutine(const sf::Time& dt)
{
    returnif (!active());

    // Update coords
    m_coords = m_inter.tileFromLocalPosition(localPosition());

    // Check for nearby heroes
    // TODO Take this range from LUA
    if (!m_fusing && isHeroNearby(0.5f)) {
        m_sprite.select((m_left)? L"lexplode" : L"rexplode");
        m_sprite.setLooping(false);
        m_fusing = true;
    }

    // Explode after fusing time
    if (m_fusing && !m_sprite.started()) {
        dungeon::Event devent;
        devent.type = "room_exploded";
        devent.action.monster = this;
        devent.action.room.x = m_coords.x;
        devent.action.room.y = m_coords.y;
        emitter()->emit(devent);
        m_fusing = false;
    }
}

//---------------------------//
//----- Node management -----//

void Creepim::setCurrentNode(const Graph::Node* node)
{
    returnif (m_currentNode == node);

    bool firstNode = (m_currentNode == nullptr);
    m_currentNode = node;

    if (m_currentNode != nullptr)
        refreshPositionFromNode(firstNode);
}

//-------------------------//
//----- Dungeon graph -----//

void Creepim::useGraph(Graph& graph)
{
    m_graph = &graph;
    m_luaActor.useGraph(graph);
}

//-------------------//
//---- Detecter -----//

/*
void Creepim::onDetecterCallback(const std::string& key)
{
    m_luaActor.state()[key]();
}
*/

//----------------------------------//
//---- Internal changes update -----//

void Creepim::refreshFromActivity()
{
    if (active()) {
        m_luaActor.reinit();
        setCurrentNode(&m_graph->node(m_inter.tileFromLocalPosition(localPosition())));
        m_sprite.setStarted(true);
    }
    else {
        setCurrentNode(nullptr);
        m_sprite.setStarted(false);
    }
}

void Creepim::refreshPositionFromNode(bool teleport)
{
    returnif (m_currentNode == nullptr);

    const auto& targetCoords = m_currentNode->coords;
    const auto tileLocalPosition = m_inter.tileLocalPosition(targetCoords);
    const auto monsterTilePosition = sf::Vector2f{m_inter.tileSize().x / 2.f, m_inter.tileSize().y * 0.62f};
    lerpable()->setTargetPosition(tileLocalPosition + monsterTilePosition);
    if (teleport) setLocalPosition(lerpable()->targetPosition());

    // TODO See Hero, same problem
    if (lerpable()->targetPosition().x > localPosition().x) {
        m_sprite.select(L"rwalk");
        m_left = false;
    }
    else if (lerpable()->targetPosition().x < localPosition().x) {
        m_sprite.select(L"lwalk");
        m_left = true;
    }
}
