#include "dungeon/monster.hpp"

#include "ai/node.hpp"
#include "dungeon/inter.hpp"
#include "tools/random.hpp"

using namespace dungeon;

Monster::Monster(Inter& inter, Graph& graph)
    : baseClass(inter, true)
    , m_graph(graph)
{
    // Lua API
    m_lua["eev_stopMoving"] = [this] { lua_stopMoving(); };
    m_lua["eev_isLookingDirection"] = [this] (const std::string& direction) { return lua_isLookingDirection(direction); };
    m_lua["eev_getCurrentRoomX"] = [this] { return lua_getCurrentRoomX(); };
    m_lua["eev_getCurrentRoomY"] = [this] { return lua_getCurrentRoomY(); };
}

//------------------//
//---- Routine -----//

void Monster::updateAI(const sf::Time& dt)
{
    returnif (!m_moving);

    // Quit if still moving
    returnif (lerpable()->positionLerping());

    // Quit if pause intended
    if (m_pauseTime >= 0.f) {
        m_pauseTime += dt.asSeconds();
        returnif (m_pauseTime < m_pauseDelay);
        m_pauseTime = -1.f;
    }

    // Get next room
    if (m_currentNode != nullptr) {
        const auto& nodeData = toNodeData(m_currentNode);
        setCurrentNode(findNextNode(nodeData)->node);
    }
}

void Monster::updateRoutine(const sf::Time& dt)
{
    massert(m_edata != nullptr, "Some Monster's elementData was left uninitialized.");

    // Update to current position
    auto relPosition = m_inter.relTileFromLocalPosition(localPosition());
    m_edata->operator[](L"rx").as_float() = relPosition.x;
    m_edata->operator[](L"ry").as_float() = relPosition.y;

    // Forward to lua
    m_lua["_update"](dt.asSeconds());
}

//-------------------------------//
//----- Graph AI evaluation -----//

const Graph::NodeData* Monster::findNextNode(const Graph::NodeData* currentNode)
{
    // That's not a node...
    returnif (currentNode == nullptr) nullptr;

    // First visit to this node
    if (m_nodeInfos[currentNode->coords].visits == 0u)
        m_lua["nonVisitedNodes"] = static_cast<uint>(m_lua["nonVisitedNodes"]) - 1u;

    m_nodeInfos[currentNode->coords].visits += 1u;
    m_nodeInfos[currentNode->coords].lastVisit = m_tick++;

    // No neighbours
    returnif (currentNode->node->neighbours.size() == 0u) currentNode;

    // Consider that the current room might be the best node
    std::vector<const Graph::NodeData*> bestNodes;
    int maxEvaluation = call("_evaluateReference", currentNode);
    bestNodes.push_back(currentNode);

    // Register evaluations
    m_evaluations.clear();
    m_evaluations.emplace_back(maxEvaluation);

    // Get the evaluation from lua
    for (const auto& neighbour : currentNode->node->neighbours) {
        auto neighbourData = reinterpret_cast<const Graph::NodeData*>(neighbour->data);
        int evaluation = call("_evaluate", neighbourData);
        m_evaluations.emplace_back(evaluation);

        // Found a new limit for the best nodes
        if (evaluation > maxEvaluation) {
            maxEvaluation = evaluation;
            bestNodes.clear();
        }

        // This node is among the best ones
        if (evaluation == maxEvaluation)
            bestNodes.emplace_back(neighbourData);
    }

    // Return a new node randomly from the best ones
    return alea::rand(bestNodes);
}

//---------------------------//
//----- LUA interaction -----//

void Monster::lua_stopMoving()
{
    m_moving = false;
    lerpable()->setTargetPosition(localPosition());
}

bool Monster::lua_isLookingDirection(const std::string& direction) const
{
    if (direction == "left")        return m_left;
    else if (direction == "right")  return !m_left;
    else throw std::runtime_error("Unknown direction '" + direction + "' from LUA isLookingDirection().");
}

uint Monster::lua_getCurrentRoomX() const
{
    return static_cast<uint>(m_edata->operator[](L"rx").as_float());
}

uint Monster::lua_getCurrentRoomY() const
{
    return static_cast<uint>(m_edata->operator[](L"ry").as_float());
}

//---------------------------//
//----- Node management -----//

void Monster::setCurrentNode(const ai::Node* node)
{
    bool firstNode = (m_currentNode == nullptr);

    // We're staying in the same room, pausing
    if (!firstNode && m_currentNode == node) {
        m_pauseTime = 0.f;
        return;
    }

    m_currentNode = node;

    if (!firstNode && m_currentNode != nullptr)
        refreshPositionFromNode();
}

//-------------------------//
//----- Dungeon graph -----//

void Monster::updateFromGraph()
{
    sf::Vector2f relCoords(m_edata->operator[](L"tx").as_float(), m_edata->operator[](L"ty").as_float());
    auto coords = sf::v2u(relCoords);
    auto node = m_graph.node(coords);

    // If the target is no more valid, find new one
    // by resetting the current node to the current position
    if (node == nullptr || toNodeData(node)->constructed == false) {
        setNewTargetPosition(localPosition());
        relCoords.x = m_edata->operator[](L"tx").as_float();
        relCoords.x = m_edata->operator[](L"ty").as_float();
        coords = sf::v2u(relCoords);
    }

    // Refresh the target position
    m_currentNode = m_graph.node(coords);
}

const Graph::NodeData* Monster::toNodeData(const ai::Node* node)
{
    returnif (node == nullptr) nullptr;
    return reinterpret_cast<const Graph::NodeData*>(node->data);
}

//-----------------------------------//
//----- Artificial intelligence -----//

void Monster::reinit()
{
    m_tick = 0u;
    m_lua["_init"]();
    m_lua["eev_nonVisitedNodes"] = m_graph.uniqueNodesCount();
    m_nodeInfos.clear();
}

Monster::Weight Monster::getWeight(const Graph::NodeData* node)
{
    Weight weight;
    weight.visited =    m_nodeInfos[node->coords].visits;
    weight.lastVisit =  m_nodeInfos[node->coords].lastVisit;
    weight.altitude =   node->altitude;
    weight.treasure =   node->treasure;
    weight.exit =       node->entrance;
    return weight;
}

uint Monster::call(const char* function, const Graph::NodeData* node)
{
    Weight weight = getWeight(node);

    m_lua["eev_weight"].SetObj(weight,
                               "visited",   &Weight::visited,
                               "lastVisit", &Weight::lastVisit,
                               "altitude",  &Weight::altitude,
                               "treasure",  &Weight::treasure,
                               "exit",      &Weight::exit);

    return m_lua[function]();
}

//-----------------------//
//---- Element data -----//

// FIXME Should that procedure be in a MovingElement class (shared with Hero)?
void Monster::bindElementData(ElementData& edata)
{
    // It's our first time is previous data is not the same monster type
    bool firstTime = (m_edata == nullptr) || (m_monsterID != edata.type());
    m_edata = &edata;

    // Get full data
    m_monsterID = m_edata->type();
    auto sMonsterID = toString(m_monsterID);
    const auto& monsterData = m_inter.monstersDB().get(m_monsterID);

    // Was there a target position saved?
    if (!m_edata->exists(L"tx")) {
        m_edata->operator[](L"tx").init_float(m_edata->operator[](L"rx").as_float());
        m_edata->operator[](L"ty").init_float(m_edata->operator[](L"ry").as_float());
    }

    // Initial position
    sf::Vector2f monsterPosition = m_inter.relTileLocalPosition({m_edata->operator[](L"rx").as_float(), m_edata->operator[](L"ry").as_float()});
    sf::Vector2f targetPosition  = m_inter.relTileLocalPosition({m_edata->operator[](L"tx").as_float(), m_edata->operator[](L"ty").as_float()});
    lerpable()->setTargetPosition(targetPosition);
    setLocalPosition(monsterPosition);
    refreshAnimation();

    // Reparameter from inter
    lerpable()->setPositionSpeed(m_inter.tileSize() * monsterData.speed);
    setDetectRangeFactor(m_inter.tileSize().x);

    // First time or new monsterID
    if (firstTime) {
        // Refresh for database
        m_pauseDelay = monsterData.pauseDelay;

        // Animated sprite
        m_sprite.load("dungeon/monsters/" + sMonsterID);

        // Lua
        std::string luaFilename = "res/ai/monsters/" + sMonsterID + ".lua";
        if (!m_lua.load(luaFilename))
            throw std::runtime_error("Failed to load Lua file: '" + luaFilename + "'. It might be a syntax error or a missing file.");

        m_lua["_register"]();
    }

    // Finish update
    updateFromGraph();
    m_lua["_reinit"]();
}

//----------------------------------//
//---- Internal changes update -----//

void Monster::setNewTargetPosition(const sf::Vector2f& targetPosition)
{
    lerpable()->setTargetPosition(targetPosition);

    auto relTargetPosition = m_inter.relTileFromLocalPosition(targetPosition);
    m_edata->operator[](L"tx").as_float() = relTargetPosition.x;
    m_edata->operator[](L"ty").as_float() = relTargetPosition.y;
}

void Monster::refreshPositionFromNode()
{
    returnif (m_currentNode == nullptr);

    const auto& targetCoords = toNodeData(m_currentNode)->coords;
    const auto tileLocalPosition = m_inter.tileLocalPosition(targetCoords);
    const auto monsterTilePosition = 0.5f * m_inter.tileSize();
    setNewTargetPosition(tileLocalPosition + monsterTilePosition);

    refreshAnimation();
}

void Monster::refreshAnimation()
{
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
