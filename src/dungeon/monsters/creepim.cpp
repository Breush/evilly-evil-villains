#include "dungeon/monsters/creepim.hpp"

#include "dungeon/inter.hpp"

using namespace dungeon::monsters;

// TODO Remove coords and use only rx/ry

Creepim::Creepim(const sf::Vector2u& coords, ElementData& elementdata, dungeon::Inter& inter)
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
    // Get next room if not already moving
    returnif (!active());
    returnif (lerpable()->positionLerping());
    setCurrentNode(m_luaActor.findNextNode(m_currentNode));
}

void Creepim::updateRoutine(const sf::Time& dt)
{
    returnif (!active());

    // Explode after fusing time
    if (m_fusing && !m_sprite.started()) {
        dungeon::Event devent;
        devent.type = "room_exploded";
        devent.action.monster = this;
        devent.action.room.x = m_explodingCoords.x;
        devent.action.room.y = m_explodingCoords.y;
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
//---- Dungeon events -----//

void Creepim::receive(const context::Event& event)
{
    const auto& devent = *reinterpret_cast<const dungeon::Event*>(&event);
    returnif (devent.type != "hero_entered");

    // If the hero is near us
    sf::Vector2u coords(devent.action.room.x, devent.action.room.y);
    if (m_inter.tileFromLocalPosition(localPosition()) == coords) {
        m_sprite.select((m_left)? L"lexplode" : L"rexplode");
        m_sprite.setLooping(false);
        m_explodingCoords = coords;
        m_fusing = true;
    }
}

//-------------------------//
//----- Dungeon graph -----//

void Creepim::useGraph(Graph& graph)
{
    m_graph = &graph;
    m_luaActor.useGraph(graph);
}

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

    m_coords = m_currentNode->coords;
    const auto tileLocalPosition = m_inter.tileLocalPosition(m_coords);
    const auto monsterTilePosition = sf::Vector2f{m_inter.tileSize().x / 2.f, m_inter.tileSize().y * 0.62f};
    lerpable()->setTargetPosition(tileLocalPosition + monsterTilePosition);
    if (teleport) setLocalPosition(lerpable()->targetPosition());

    // TODO Changing room, we need to check if there is some hero in it
    // and explode if so.

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
