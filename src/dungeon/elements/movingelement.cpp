#include "dungeon/elements/movingelement.hpp"

#include "ai/node.hpp"
#include "dungeon/inter.hpp"
#include "scene/components/lerpable.hpp"
#include "tools/random.hpp"

using namespace dungeon;

MovingElement::MovingElement(std::string folder, Inter& inter, Graph& graph)
    : baseClass(inter)
    , m_graph(graph)
    , m_folder(std::move(folder))
{
    addComponent<scene::Lerpable>(*this);

    // Lua API
    lua()["eev_setMoving"] = [this] (bool moving) { lua_setMoving(moving); };
    lua()["eev_isLookingDirection"] = [this] (const std::string& direction) { return lua_isLookingDirection(direction); };
    lua()["eev_getCurrentRoomX"] = [this] { return lua_getCurrentRoomX(); };
    lua()["eev_getCurrentRoomY"] = [this] { return lua_getCurrentRoomY(); };
}

MovingElement::~MovingElement()
{
    removeComponent<scene::Lerpable>();
}

//------------------//
//---- Routine -----//

void MovingElement::updateAI(const sf::Time& dt)
{
    returnif (!m_moving);

    // Quit if still moving
    returnif (getComponent<scene::Lerpable>()->positionLerping());

    // Quit if pause intended
    if (m_pauseTime >= 0.f) {
        m_pauseTime += dt.asSeconds();
        returnif (m_pauseTime < m_pauseDelay);
        m_pauseTime = -1.f;
    }

    // Get next room
    if (m_currentNode != nullptr)
        setCurrentNode(findNextNode(m_currentNode));

    // Forward to lua
    lua()["_update"](dt.asSeconds());
}

void MovingElement::updateRoutine(const sf::Time& dt)
{
    // Update to current position
    auto relPosition = m_inter.relCoordsFromPosition(localPosition());
    m_edata->operator[](L"rx").as_float() = relPosition.x;
    m_edata->operator[](L"ry").as_float() = relPosition.y;

    baseClass::updateRoutine(dt);
    updateAI(dt);
}

//-------------------------------//
//----- Graph AI evaluation -----//

MovingElement::NodeWay MovingElement::findNextNode(const Graph::NodeData* currentNode)
{
    NodeWay nodeWay;

    // That's not a node...
    returnif (currentNode == nullptr) nodeWay;

    // First visit to this node
    if (m_nodeInfos[currentNode->coords].visits == 0u)
        lua()["nonVisitedNodes"] = static_cast<uint>(lua()["nonVisitedNodes"]) - 1u;

    m_nodeInfos[currentNode->coords].visits += 1u;
    m_nodeInfos[currentNode->coords].lastVisit = m_tick++;

    // No neighbours
    nodeWay.nodeData = currentNode;
    returnif (currentNode->node->neighbours.size() == 0u) nodeWay;

    // Consider that the current room might be the best node
    std::vector<NodeWay> bestNodes;
    int maxEvaluation = call("_evaluateReference", currentNode);
    bestNodes.emplace_back(std::move(nodeWay));

    // Get the evaluation from lua
    for (const auto& neighbour : currentNode->node->neighbours) {
        auto nodeData = reinterpret_cast<const Graph::NodeData*>(neighbour.node->data);
        int evaluation = call("_evaluate", nodeData);

        // Found a new limit for the best nodes
        if (evaluation > maxEvaluation) {
            maxEvaluation = evaluation;
            bestNodes.clear();
        }

        // This node is among the best ones
        if (evaluation == maxEvaluation) {
            auto neighbourData = reinterpret_cast<const Graph::NeighbourData*>(neighbour.data);
            nodeWay.nodeData = nodeData;
            nodeWay.neighbourData = neighbourData;
            bestNodes.emplace_back(std::move(nodeWay));
        }
    }

    // Return a new node randomly from the best ones
    return alea::rand(bestNodes);
}

//---------------------------//
//----- LUA interaction -----//

void MovingElement::lua_setMoving(bool moving)
{
    setMoving(moving);
}

bool MovingElement::lua_isLookingDirection(const std::string& direction) const
{
    if (direction == "left")        return m_left;
    else if (direction == "right")  return !m_left;
    else throw std::runtime_error("Unknown direction '" + direction + "' from LUA isLookingDirection().");
}

//---------------------------//
//----- Node management -----//

void MovingElement::setMoving(bool moving)
{
    m_moving = moving;
    setNewTargetPosition(localPosition());
    getComponent<scene::Lerpable>()->setPositionLerping(moving);
    if (moving) updateFromGraph();
}

void MovingElement::setCurrentNode(const NodeWay& nodeWay)
{
    bool firstNode = (m_currentNode == nullptr);

    if (!firstNode) {
        // We are getting out of a tunnel, warn the orgin facility
        if (m_inTunnel) {
            auto tunnelOriginFacility = m_inter.findRoomFacility(m_tunnelOriginCoords, m_tunnelOriginFacilityID);
            if (tunnelOriginFacility != nullptr) tunnelOriginFacility->movingElementLeaveTunnel(*this);
            else resetClipAreas();
            m_inTunnel = false;
        }

        // We're staying in the same room, pausing
        if (m_currentNode == nodeWay.nodeData) {
            m_pauseTime = 0.f;
            return;
        }

        if (nodeWay.neighbourData != nullptr)
        {
            // We are going to take a tunnel, warn the facility
            m_tunnelOriginFacilityID = nodeWay.neighbourData->tunnelFacilityID;
            if (!m_tunnelOriginFacilityID.empty()) {
                m_inTunnel = true;
                m_tunnelOriginCoords = m_currentNode->coords;
                m_inter.findRoomFacility(m_tunnelOriginCoords, m_tunnelOriginFacilityID)->movingElementEnterTunnel(*this);
            }
        }
    }

    m_currentNode = nodeWay.nodeData;

    if (!firstNode && m_currentNode != nullptr)
        refreshPositionFromNode();
}

//-------------------------//
//----- Dungeon graph -----//

void MovingElement::updateFromGraph()
{
    RoomRelCoords relCoords(m_edata->operator[](L"tx").as_float(), m_edata->operator[](L"ty").as_float());
    auto coords = toCoords(relCoords);
    auto nodeData = m_graph.nodeData(coords);

    // If the target is no more valid, find new one
    // by resetting the current node to the current position
    if (nodeData == nullptr || nodeData->constructed == false) {
        setNewTargetPosition(localPosition());
        relCoords.x = m_edata->operator[](L"tx").as_float();
        relCoords.y = m_edata->operator[](L"ty").as_float();
        coords = toCoords(relCoords);
        nodeData = m_graph.nodeData(coords);
    }

    // Refresh the pointer
    m_currentNode = nodeData;
    m_pauseTime = -1.f;
}

//-----------------------------------//
//----- Artificial intelligence -----//

void MovingElement::reinit()
{
    m_tick = 0u;
    lua()["_init"]();
    lua()["eev_nonVisitedNodes"] = m_graph.uniqueNodesCount();
    m_nodeInfos.clear();
}

Monster::Weight MovingElement::getWeight(const Graph::NodeData* node)
{
    Weight weight;
    weight.visited =    m_nodeInfos[node->coords].visits;
    weight.lastVisit =  m_nodeInfos[node->coords].lastVisit;
    weight.altitude =   node->altitude;
    weight.treasure =   node->treasure;
    weight.exit =       node->entrance;
    return weight;
}

uint MovingElement::call(const char* function, const Graph::NodeData* node)
{
    Weight weight = getWeight(node);

    lua()["eev_weight"].SetObj(weight,
                               "visited",   &Weight::visited,
                               "lastVisit", &Weight::lastVisit,
                               "altitude",  &Weight::altitude,
                               "treasure",  &Weight::treasure,
                               "exit",      &Weight::exit);

    return lua()[function]();
}

//-----------------------//
//---- Element data -----//

void MovingElement::bindElementData(ElementData& edata)
{
    // It's our first time is previous data is not the same element type
    bool firstTime = (m_edata == nullptr) || (m_elementID != edata.type());
    m_edata = &edata;

    // Get full data
    m_elementID = m_edata->type();
    auto sElementID = toString(m_elementID);
    rebindElementData();

    // Was there a target position saved?
    if (!m_edata->exists(L"tx")) {
        m_edata->operator[](L"tx").init_float(m_edata->operator[](L"rx").as_float());
        m_edata->operator[](L"ty").init_float(m_edata->operator[](L"ry").as_float());
    }

    // Initial position
    refreshPosition();
    refreshAnimation();

    // Reparameter from inter
    setDetectRangeFactor(m_inter.tileSize().x);

    // First time or new elementID
    if (firstTime) {
        // Animated sprite
        m_sprite.load(m_folder + sElementID + "/anim");

        // Lua
        std::string luaFilename = "res/" + m_folder + sElementID + "/ai.lua";
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
    updateFromGraph();
    lua()["_reinit"]();
}

void MovingElement::refreshPosition()
{
    returnif (m_edata == nullptr);

    sf::Vector2f position = m_inter.positionFromRelCoords({m_edata->operator[](L"rx").as_float(), m_edata->operator[](L"ry").as_float()});
    sf::Vector2f targetPosition  = m_inter.positionFromRelCoords({m_edata->operator[](L"tx").as_float(), m_edata->operator[](L"ty").as_float()});
    getComponent<scene::Lerpable>()->setTargetPosition(targetPosition);
    setLocalPosition(position);
}

//----------------------------------//
//---- Internal changes update -----//

void MovingElement::setNewTargetPosition(const sf::Vector2f& targetPosition)
{
    getComponent<scene::Lerpable>()->setTargetPosition(targetPosition);

    auto relTargetPosition = m_inter.relCoordsFromPosition(targetPosition);
    m_edata->operator[](L"tx").as_float() = relTargetPosition.x;
    m_edata->operator[](L"ty").as_float() = relTargetPosition.y;
}

void MovingElement::refreshPositionFromNode()
{
    returnif (m_currentNode == nullptr);

    const auto& targetCoords = m_currentNode->coords;
    const auto tileLocalPosition = m_inter.positionFromRoomCoords(targetCoords);
    const auto monsterTilePosition = 0.5f * m_inter.tileSize();
    setNewTargetPosition(tileLocalPosition + monsterTilePosition);

    refreshAnimation();
}

void MovingElement::refreshAnimation()
{
    returnif (m_inTunnel);

    auto lerpable = getComponent<scene::Lerpable>();

    // Select correct animation (right/left)
    // TODO Have a state machine and interface with physics module
    // Maybe the state machine animated sprite makes its own class
    if (lerpable->targetPosition().x > localPosition().x) {
        m_sprite.select("rwalk");
        m_left = false;
    }
    else if (lerpable->targetPosition().x < localPosition().x) {
        m_sprite.select("lwalk");
        m_left = true;
    }
}
