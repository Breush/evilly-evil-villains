#include "dungeon/monster.hpp"

#include "ai/node.hpp"
#include "dungeon/inter.hpp"
#include "dungeon/monstersdb.hpp"
#include "tools/random.hpp"

using namespace dungeon;
using namespace std::placeholders;

Monster::Monster(ElementData& edata, Inter& inter)
    : baseClass(true)
    , m_active(false)
    , m_inter(inter)
    , m_edata(edata)
{
    setDetectable(false);
    const auto& monsterID = m_edata.type();
    auto sMonsterID = toString(monsterID);
    const auto& monsterData = m_inter.monstersDB().get(monsterID);

    // Initializing
    lerpable()->setPositionSpeed(m_inter.tileSize() * monsterData.speed);
    setDetectRangeFactor(m_inter.tileSize().x);
    m_pauseDelay = monsterData.pauseDelay;

    // Decorum
    attachChild(m_sprite);
    m_sprite.load("dungeon/monsters/" + sMonsterID);
    m_sprite.setStarted(false);
    m_sprite.setLocalScale(m_inter.roomScale());

    // Was there a target position saved?
    if (!m_edata.exists(L"tx")) {
        m_edata[L"tx"].init_float(m_edata[L"rx"].as_float());
        m_edata[L"ty"].init_float(m_edata[L"ry"].as_float());
    }

    // Initial position
    sf::Vector2f monsterPosition = m_inter.relTileLocalPosition({m_edata[L"rx"].as_float(), m_edata[L"ry"].as_float()});
    sf::Vector2f targetPosition = m_inter.relTileLocalPosition({m_edata[L"tx"].as_float(), m_edata[L"ty"].as_float()});
    lerpable()->setTargetPosition(targetPosition);
    setLocalPosition(monsterPosition);
    refreshAnimation();
    centerOrigin();

    // Lua
    std::string luaFilename = "res/ai/monsters/" + sMonsterID + ".lua";
    if (!m_lua.load(luaFilename))
        throw std::runtime_error("Failed to load Lua file: '" + luaFilename + "'. It might be a syntax error or a missing file.");

    // Lua API
    std::function<void(const std::string&, const std::string&, const std::string&)> eev_addCallback = std::bind(&Monster::lua_addCallback, this, _1, _2, _3);
    m_lua["eev_addCallback"] = eev_addCallback;
    m_lua["eev_stopMoving"] = [this] { lua_stopMoving(); };
    m_lua["eev_setAnimationLooping"] = [this] (const bool looping) { lua_setAnimationLooping(looping); };
    m_lua["eev_selectAnimation"] = [this] (const std::string& animationKey) { lua_selectAnimation(animationKey); };
    m_lua["eev_isLookingDirection"] = [this] (const std::string& direction) { return lua_isLookingDirection(direction); };
    m_lua["eev_isAnimationStopped"] = [this] { return lua_isAnimationStopped(); };
    m_lua["eev_forwardAnimation"] = [this] (const lua_Number offset) { lua_forwardAnimation(offset); };
    m_lua["eev_getCurrentRoomX"] = [this] { return lua_getCurrentRoomX(); };
    m_lua["eev_getCurrentRoomY"] = [this] { return lua_getCurrentRoomY(); };
    m_lua["eev_log"] = [this] (const std::string& str) { lua_log(str); };
    m_lua["eev_dungeonExplodeRoom"] = [this] (const uint x, const uint y) { lua_dungeonExplodeRoom(x, y); };

    m_lua["eev_setDataFloat"] = [this] (const std::string& s, const lua_Number value) { lua_setDataFloat(s, value); };
    m_lua["eev_getDataFloat"] = [this] (const std::string& s) { return lua_getDataFloat(s); };
    m_lua["eev_initEmptyDataFloat"] = [this] (const std::string& s, const lua_Number value) { lua_initEmptyDataFloat(s, value); };

    // Call for all things to register
    m_lua["_register"]();

    refreshFromActivity();
}

//------------------//
//---- Routine -----//

void Monster::onTransformChanges()
{
    auto relPosition = m_inter.relTileFromLocalPosition(localPosition());
    m_edata[L"rx"].as_float() = relPosition.x;
    m_edata[L"ry"].as_float() = relPosition.y;
}

void Monster::updateAI(const sf::Time& dt)
{
    returnif (!m_moving);
    returnif (!active());

    // Quit if still moving
    returnif (lerpable()->positionLerping());

    // Quit if pause intended
    if (m_pauseTime >= 0.f) {
        m_pauseTime += dt.asSeconds();
        returnif (m_pauseTime < m_pauseDelay);
        m_pauseTime = -1.f;
    }

    // Get next room if not already moving
    setCurrentNode(findNextNode(toNodeData(m_currentNode))->node);
}

void Monster::updateRoutine(const sf::Time& dt)
{
    returnif (!active());

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

void Monster::lua_addCallback(const std::string& luaKey, const std::string& entityType, const std::string& condition)
{
    addDetectSignal(entityType, condition, [this, luaKey] (const uint32 UID) { m_lua[luaKey.c_str()](UID); });
}

void Monster::lua_initEmptyDataFloat(const std::string& s, const lua_Number value)
{
    auto ws = toWString(s);
    if (!m_edata.exists(ws))
        m_edata[ws].init_float(static_cast<float>(value));
}

void Monster::lua_setDataFloat(const std::string& s, const lua_Number value)
{
    auto ws = toWString(s);
    m_edata[ws].as_float() = static_cast<float>(value);
}

lua_Number Monster::lua_getDataFloat(const std::string& s) const
{
    auto ws = toWString(s);
    return static_cast<lua_Number>(m_edata.at(ws).as_float());
}

void Monster::lua_selectAnimation(const std::string& animationKey)
{
    m_sprite.select(toWString(animationKey));
}

void Monster::lua_setAnimationLooping(const bool looping)
{
    m_sprite.setLooping(looping);
}

void Monster::lua_forwardAnimation(const lua_Number offset)
{
    m_sprite.forward(sf::seconds(static_cast<float>(offset)));
}

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

bool Monster::lua_isAnimationStopped() const
{
    return !m_sprite.started();
}

uint Monster::lua_getCurrentRoomX() const
{
    return static_cast<uint>(m_edata[L"rx"].as_float());
}

uint Monster::lua_getCurrentRoomY() const
{
    return static_cast<uint>(m_edata[L"ry"].as_float());
}

void Monster::lua_log(const std::string& str) const
{
    std::cerr << "LUA [monster::" << toString(m_edata.type()) << "] " << str << std::endl;
}

void Monster::lua_dungeonExplodeRoom(const uint x, const uint y)
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

void Monster::useGraph(Graph& graph)
{
    m_graph = &graph;
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
    m_lua["eev_nonVisitedNodes"] = m_graph->uniqueNodesCount();
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

//----------------------------------//
//---- Internal changes update -----//

void Monster::refreshFromActivity()
{
    if (active()) {
        reinit();
        m_sprite.setStarted(true);
        sf::Vector2f relCoords(m_edata[L"tx"].as_float(), m_edata[L"ty"].as_float());
        auto targetPosition = m_inter.relTileLocalPosition(relCoords);
        lerpable()->setTargetPosition(targetPosition);
        m_currentNode = &m_graph->node(sf::v2u(relCoords));
    }
    else {
        setCurrentNode(nullptr);
        m_sprite.setStarted(false);
        lerpable()->setTargetPosition(localPosition());
    }
}

void Monster::refreshPositionFromNode()
{
    returnif (m_currentNode == nullptr);

    const auto& targetCoords = toNodeData(m_currentNode)->coords;
    const auto tileLocalPosition = m_inter.tileLocalPosition(targetCoords);
    const auto monsterTilePosition = 0.5f * m_inter.tileSize();
    const auto targetPosition = tileLocalPosition + monsterTilePosition;
    lerpable()->setTargetPosition(targetPosition);

    auto relTargetPosition = m_inter.relTileFromLocalPosition(targetPosition);
    m_edata[L"tx"].as_float() = relTargetPosition.x;
    m_edata[L"ty"].as_float() = relTargetPosition.y;

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
