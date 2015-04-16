#include "dungeon/hero.hpp"

#include "dungeon/inter.hpp"
#include "dungeon/data.hpp"
#include "tools/random.hpp"
#include "tools/debug.hpp"
#include "tools/tools.hpp"

#include <stdexcept> // runtime_error

using namespace dungeon;

Hero::Hero(const Inter* inter)
    : m_running(false)
    , m_inter(inter)
{
    // Sprite
    m_sprite.setSize({10.f, 10.f});
    m_sprite.setFillColor(sf::Color::White);

    // Lua
    if (!m_lua.load("res/ai/hero.lua"))
        throw std::runtime_error("Failed to load hero AI.");
}

//-------------------//
//----- Routine -----//

void Hero::updateAI(const sf::Time& dt)
{
    returnif (!m_running);

    m_inRoomSince += dt.asSeconds();

    // TODO Interface with physics engine

    // Look for next room each two seconds
    if (m_inRoomSince >= 2.f) {
        m_inRoomSince -= 2.f;
        returnif (m_currentNode->neighbours.size() == 0u);

        // Consider that the current room might be the best node
        std::vector<const Graph::Node*> bestNodes;
        bestNodes.push_back(m_currentNode);
        int maxEvaluation = call("evaluate_reference", m_currentNode);

        // Get the evaluation from lua
        for (const auto& neighbour : m_currentNode->neighbours) {
            int evaluation = call("evaluate", neighbour);

            // Found a new limit for the best nodes
            if (evaluation > maxEvaluation) {
                maxEvaluation = evaluation;
                bestNodes.clear();
            }

            // This node is among the best ones
            if (evaluation == maxEvaluation)
                bestNodes.push_back(neighbour);
        }

        // Switch to a new node randomly from the best ones
        m_currentNode = alea::rand(bestNodes);
        m_visitedNodes[m_currentNode->room] += 1u;

        refreshPositionFromNode();
    }
}

//-----------------------------------//
//----- Artificial intelligence -----//

uint Hero::call(const char* function, const Graph::Node* node)
{
    Weight weight;
    weight.altitude = node->altitude;
    weight.visited = m_visitedNodes[node->room];
    m_lua["weight"].SetObj(weight,
                           "altitude", &Weight::altitude,
                           "visited", &Weight::visited);
    return m_lua[function]();
}

//-------------------------//
//----- Dungeon graph -----//

void Hero::useGraph(Graph& graph)
{
    m_graph = &graph;
}

//------------------//
//----- Events -----//

void Hero::receive(const Event& event)
{
    returnif (event.type != EventType::MODE_CHANGED);
    setRunning(event.mode == Mode::INVASION);
}

//-----------------------------------//
//----- Internal change updates -----//

void Hero::changedRunning()
{
    clearParts();

    if (m_running) {
        // Make it visible
        addPart(&m_sprite);

        // FIXME It's a bit strange to control the graph from here.
        m_graph->reconstructFromData();

        // Get the door from the graph.
        m_visitedNodes.clear();
        m_currentNode = &m_graph->startingNode();
        m_visitedNodes[m_currentNode->room] += 1u;
        refreshPositionFromNode();
    }
}

void Hero::refreshPositionFromNode()
{
    setLocalPosition(m_inter->roomLocalPosition(m_currentNode->room) + m_inter->roomSize() / 2.f);
}
