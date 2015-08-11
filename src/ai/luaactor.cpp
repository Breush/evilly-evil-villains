#include "ai/luaactor.hpp"

#include "tools/random.hpp"
#include "tools/tools.hpp"

using namespace ai;

//-------------------//
//----- Control -----//

const dungeon::Graph::Node* LuaActor::findNextNode(const Node* currentNode)
{
    // That's not a node...
    returnif (currentNode == nullptr) nullptr;

    // First visit to this node
    if (m_nodeInfos[currentNode->coords].visits == 0u)
        m_lua["nonVisitedNodes"] = static_cast<uint>(m_lua["nonVisitedNodes"]) - 1u;

    m_nodeInfos[currentNode->coords].visits += 1u;
    m_nodeInfos[currentNode->coords].lastVisit = m_tick++;

    // No neighbours
    returnif (currentNode->neighbours.size() == 0u) currentNode;

    // Consider that the current room might be the best node
    std::vector<const Graph::Node*> bestNodes;
    bestNodes.push_back(currentNode);
    int maxEvaluation = call("evaluate_reference", currentNode);

    // Register evaluations
    m_evaluations.clear();
    m_evaluations.emplace_back(maxEvaluation);

    // If the hero escaped during evaluation, stop it here
    // TODO This should be a pending state in Hero code... nothing to check here!
    returnif (currentNode == nullptr) nullptr;

    // Get the evaluation from lua
    for (const auto& neighbour : currentNode->neighbours) {
        int evaluation = call("evaluate", neighbour);
        m_evaluations.emplace_back(evaluation);

        // Found a new limit for the best nodes
        if (evaluation > maxEvaluation) {
            maxEvaluation = evaluation;
            bestNodes.clear();
        }

        // This node is among the best ones
        if (evaluation == maxEvaluation)
            bestNodes.emplace_back(neighbour);
    }

    // Return a new node randomly from the best ones
    return alea::rand(bestNodes);
}

//-------------------------//
//----- Configuration -----//

void LuaActor::reinit()
{
    m_tick = 0u;
    m_lua["init"]();
    m_lua["nonVisitedNodes"] = m_graph->uniqueNodesCount();
    m_nodeInfos.clear();
}

void LuaActor::loadFile(const std::string& filename)
{
    if (!m_lua.load(filename))
        throw std::runtime_error("Failed to load Lua file.");
}

void LuaActor::useGraph(Graph& graph)
{
    m_graph = &graph;
}

//-----------------------------------//
//----- Artificial intelligence -----//

LuaActor::Weight LuaActor::getWeight(const dungeon::Graph::Node* node)
{
    Weight weight;
    weight.visited =    m_nodeInfos[node->coords].visits;
    weight.lastVisit =  m_nodeInfos[node->coords].lastVisit;
    weight.altitude =   node->altitude;
    weight.treasure =   node->treasure;
    weight.exit =       node->entrance;
    return weight;
}

uint LuaActor::call(const char* function, const dungeon::Graph::Node* node)
{
    Weight weight = getWeight(node);

    m_lua["weight"].SetObj(weight,
                           "visited",   &Weight::visited,
                           "lastVisit", &Weight::lastVisit,
                           "altitude",  &Weight::altitude,
                           "treasure",  &Weight::treasure,
                           "exit",      &Weight::exit);

    return m_lua[function]();
}
