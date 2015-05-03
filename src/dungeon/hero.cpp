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
    setVisible(false);

    // Sprite
    m_sprite.setSize({10.f, 10.f});
    m_sprite.setFillColor(sf::Color::White);
    addPart(&m_sprite);

    // Lua
    if (!m_lua.load("res/ai/hero.lua"))
        throw std::runtime_error("Failed to load hero AI.");

    // Register
    m_lua["AIGetOut"] = [this] { AIGetOut(); };
    m_lua["AIStealTreasure"] = [this] { AIStealTreasure(); };
}

//-------------------//
//----- Routine -----//

void Hero::updateAI(const sf::Time& dt)
{
    returnif (!m_running);

    m_inRoomSince += dt.asSeconds();

    // TODO Interface with physics engine

    // Look for next room
    const float m_timeInEachRoom = 0.1f;
    if (m_inRoomSince >= m_timeInEachRoom) {
        m_inRoomSince -= m_timeInEachRoom;
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
        m_nodeInfos[m_currentNode->coords].visits += 1u;
        m_nodeInfos[m_currentNode->coords].lastVisit = m_tick;

        refreshPositionFromNode();
        ++m_tick;
    }
}

//-----------------------------------//
//----- Artificial intelligence -----//

uint Hero::call(const char* function, const Graph::Node* node)
{
    Weight weight;
    weight.visited =    m_nodeInfos[node->coords].visits;
    weight.lastVisit =  m_nodeInfos[node->coords].lastVisit;
    weight.altitude =   node->altitude;
    weight.treasure =   node->treasure;
    weight.exit =       node->entrance;

    m_lua["weight"].SetObj(weight,
                           "visited",   &Weight::visited,
                           "lastVisit", &Weight::lastVisit,
                           "altitude",  &Weight::altitude,
                           "treasure",  &Weight::treasure,
                           "exit",      &Weight::exit);

    return m_lua[function]();
}

void Hero::AIGetOut()
{
    returnif (!m_currentNode->entrance);

    std::cerr << this << " gets out." << std::endl;
    setRunning(false);
}

void Hero::AIStealTreasure()
{
    auto maxStolenDosh = std::max(100u, m_currentNode->treasure);
    auto stolenDosh = 1u + rand() % maxStolenDosh;
    m_data->stealTreasure(m_currentNode->coords, *this, stolenDosh);

    std::cerr << this << " steals " << stolenDosh << " dosh from treasure in room " << m_currentNode->coords << "." << std::endl;
}

//-------------------------//
//----- Dungeon graph -----//

void Hero::useGraph(Graph& graph)
{
    m_graph = &graph;
}

//--------------------------//
//----- Dungeon events -----//

void Hero::useData(Data& data)
{
    m_data = &data;
    setEmitter(&data);
}

void Hero::receive(const Event& event)
{
    returnif (event.type != EventType::MODE_CHANGED);
    setRunning(event.mode == Mode::INVASION);
}

//-----------------------------------//
//----- Internal change updates -----//

void Hero::changedRunning()
{
    setVisible(m_running);

    if (m_running) {
        // Reinitialize AI
        m_tick = 0u;
        m_nodeInfos.clear();
        m_lua["init"]();

        // Get the door from the graph (requires that it is correctly constructed).
        m_currentNode = &m_graph->startingNode();
        m_nodeInfos[m_currentNode->coords].visits += 1u;
        m_nodeInfos[m_currentNode->coords].lastVisit = 0u;
        refreshPositionFromNode();
    }
}

void Hero::refreshPositionFromNode()
{
    setLocalPosition(m_inter->tileLocalPosition(m_currentNode->coords) + m_inter->tileSize() / 2.f);
}
