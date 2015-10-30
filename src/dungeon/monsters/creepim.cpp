#include "dungeon/monsters/creepim.hpp"

#include "ai/node.hpp"
#include "dungeon/inter.hpp"
#include "tools/random.hpp"

using namespace dungeon::monsters;
using namespace std::placeholders;

Creepim::Creepim(ElementData& elementdata, dungeon::Inter& inter)
    : baseClass(elementdata, inter)
{
    // TODO Where to get speed from? (Maybe MonstersDB)
    lerpable()->setPositionSpeed({50.f, 25.f});

    // Initial position
    sf::Vector2f monsterPosition;
    monsterPosition.x = m_elementdata[L"ry"].as_float() * m_inter.tileSize().x;
    monsterPosition.y = m_inter.size().y - m_elementdata[L"rx"].as_float() * m_inter.tileSize().y;
    lerpable()->setTargetPosition(monsterPosition);
    setLocalPosition(monsterPosition);

    // Decorum
    attachChild(m_sprite);
    m_sprite.load("dungeon/monsters/creepim");
    m_sprite.setStarted(false);
    m_sprite.setLocalScale(m_inter.roomScale());

    // Lua
    std::string luaFilename = "res/ai/monsters/creepim.lua";
    if (!m_lua.load(luaFilename))
        throw std::runtime_error("Failed to load Lua file: '" + luaFilename + "'. It might be a syntax error.");

    // All moving elements inside dungeon should become dungeon::DetectEntity
    // TODO Move to register through the dungeon detecter (function inherited from dungeon::DetectEntity)
    std::function<void(const std::string&, const std::string&, const std::string&)> eev_addCallback = std::bind(&Creepim::lua_addCallback, this, _1, _2, _3);
    m_lua["eev_addCallback"] = eev_addCallback;

    // Lua API
    m_lua["eev_stopMoving"] = [this] { lua_stopMoving(); };
    m_lua["eev_setAnimationLooping"] = [this] (const bool looping) { lua_setAnimationLooping(looping); };
    m_lua["eev_selectAnimation"] = [this] (const std::string& animationKey) { lua_selectAnimation(animationKey); };
    m_lua["eev_isLookingDirection"] = [this] (const std::string& direction) { return lua_isLookingDirection(direction); };
    m_lua["eev_isAnimationStopped"] = [this] { return lua_isAnimationStopped(); };
    m_lua["eev_getCurrentRoomX"] = [this] { return lua_getCurrentRoomX(); };
    m_lua["eev_getCurrentRoomY"] = [this] { return lua_getCurrentRoomY(); };
    m_lua["eev_log"] = [this] (const std::string& str) { lua_log(str); };
    m_lua["eev_dungeonExplodeRoom"] = [this] (const uint x, const uint y) { lua_dungeonExplodeRoom(x, y); };

    // Call for all things to register
    m_lua["_register"]();

    refreshFromActivity();
}

//------------------//
//---- Routine -----//

void Creepim::onTransformChanges()
{
    // Note: Room coordinates convention is inverted because the floor appears first
    m_elementdata[L"rx"].as_float() = (m_inter.size().y - localPosition().y) / m_inter.tileSize().y;
    m_elementdata[L"ry"].as_float() = localPosition().x / m_inter.tileSize().x;
}

void Creepim::updateAI(const sf::Time& dt)
{
    returnif (!m_moving);
    returnif (!active());
    returnif (lerpable()->positionLerping());

    // Get next room if not already moving
    setCurrentNode(findNextNode(toNodeData(m_currentNode))->node);
}

void Creepim::updateRoutine(const sf::Time& dt)
{
    returnif (!active());

    // TODO We're currently simulating the callback registered
    if (isHeroNearby(0.5f))
        m_lua["cbHeroClose"]();

    // Forward to lua
    m_lua["_update"](dt.asSeconds());
}

//-------------------------------//
//----- Graph AI evaluation -----//

const dungeon::Graph::NodeData* Creepim::findNextNode(const dungeon::Graph::NodeData* currentNode)
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
    std::vector<const dungeon::Graph::NodeData*> bestNodes;
    int maxEvaluation = call("_evaluateReference", currentNode);
    bestNodes.push_back(currentNode);

    // Register evaluations
    m_evaluations.clear();
    m_evaluations.emplace_back(maxEvaluation);

    // Get the evaluation from lua
    for (const auto& neighbour : currentNode->node->neighbours) {
        auto neighbourData = reinterpret_cast<const dungeon::Graph::NodeData*>(neighbour->data);
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

void Creepim::lua_addCallback(const std::string& luaKey, const std::string& entityType, const std::string& condition)
{
    // TODO Have it analyzed etc.
    std::cerr << "TODO Should register: " << luaKey << " " << entityType << " " << condition << std::endl;
}

void Creepim::lua_selectAnimation(const std::string& animationKey)
{
    m_sprite.select(toWString(animationKey));
}

void Creepim::lua_setAnimationLooping(const bool looping)
{
    m_sprite.setLooping(looping);
}

void Creepim::lua_stopMoving()
{
    m_moving = false;
    lerpable()->setTargetPosition(localPosition());
}

bool Creepim::lua_isLookingDirection(const std::string& direction) const
{
    if (direction == "left")        return m_left;
    else if (direction == "right")  return !m_left;
    else throw std::runtime_error("Unknown direction '" + direction + "' from LUA isLookingDirection().");
}

bool Creepim::lua_isAnimationStopped() const
{
    return !m_sprite.started();
}

uint Creepim::lua_getCurrentRoomX() const
{
    return static_cast<uint>(m_elementdata[L"rx"].as_float());
}

uint Creepim::lua_getCurrentRoomY() const
{
    return static_cast<uint>(m_elementdata[L"ry"].as_float());
}

void Creepim::lua_log(const std::string& str) const
{
    std::cerr << "LUA: " << str << std::endl;
}

void Creepim::lua_dungeonExplodeRoom(const uint x, const uint y)
{
    dungeon::Event devent;
    devent.type = "room_exploded";
    devent.action.monster = this;
    devent.action.room.x = x;
    devent.action.room.y = y;
    emitter()->emit(devent);
}

//---------------------------//
//----- Node management -----//

void Creepim::setCurrentNode(const ai::Node* node)
{
    returnif (m_currentNode == node);

    bool firstNode = (m_currentNode == nullptr);
    m_currentNode = node;

    if (!firstNode && m_currentNode != nullptr)
        refreshPositionFromNode();
}

//-------------------------//
//----- Dungeon graph -----//

void Creepim::useGraph(Graph& graph)
{
    m_graph = &graph;
}

const dungeon::Graph::NodeData* Creepim::toNodeData(const ai::Node* node)
{
    returnif (node == nullptr) nullptr;
    return reinterpret_cast<const Graph::NodeData*>(node->data);
}

//-----------------------------------//
//----- Artificial intelligence -----//

void Creepim::reinit()
{
    m_tick = 0u;
    m_lua["_init"]();
    m_lua["eev_nonVisitedNodes"] = m_graph->uniqueNodesCount();
    m_nodeInfos.clear();
}

Creepim::Weight Creepim::getWeight(const dungeon::Graph::NodeData* node)
{
    Weight weight;
    weight.visited =    m_nodeInfos[node->coords].visits;
    weight.lastVisit =  m_nodeInfos[node->coords].lastVisit;
    weight.altitude =   node->altitude;
    weight.treasure =   node->treasure;
    weight.exit =       node->entrance;
    return weight;
}

uint Creepim::call(const char* function, const dungeon::Graph::NodeData* node)
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

//----------------------------------//
//---- Internal changes update -----//

void Creepim::refreshFromActivity()
{
    if (active()) {
        reinit();
        setCurrentNode(&m_graph->node(m_inter.tileFromLocalPosition(localPosition())));
        m_sprite.setStarted(true);
    }
    else {
        setCurrentNode(nullptr);
        m_sprite.setStarted(false);
        lerpable()->setTargetPosition(localPosition());
    }
}

void Creepim::refreshPositionFromNode()
{
    returnif (m_currentNode == nullptr);

    const auto& targetCoords = toNodeData(m_currentNode)->coords;
    const auto tileLocalPosition = m_inter.tileLocalPosition(targetCoords);
    const auto monsterTilePosition = 0.5f * m_inter.tileSize();
    lerpable()->setTargetPosition(tileLocalPosition + monsterTilePosition);

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
