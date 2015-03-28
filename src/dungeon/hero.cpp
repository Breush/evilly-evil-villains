#include "dungeon/hero.hpp"

#include "dungeon/inter.hpp"
#include "tools/random.hpp"
#include "tools/debug.hpp"
#include "tools/tools.hpp"

#include <stdexcept> // runtime_error

using namespace dungeon;

Hero::Hero(const Inter* inter)
    : m_inter(inter)
{
    // Sprite
    m_sprite.setSize({10.f, 10.f});
    m_sprite.setFillColor(sf::Color::White);
    addPart(&m_sprite);

    // Lua
    if (!m_lua.load("res/ai/hero.lua"))
        throw std::runtime_error("Failed to load hero AI.");
}

//-------------------//
//----- Routine -----//

void Hero::updateAI(const sf::Time& dt)
{
    m_inRoomSince += dt.asSeconds();

    // TODO Interface with physics engine
    // TODO Export to Lua

    // Look for next room each two seconds
    if (m_inRoomSince >= 2.f) {
        m_inRoomSince -= 2.f;
        returnif (m_currentNode->neighbours.size() == 0u);

        // TODO Find a way to pass better arguments to lua
        // Like directly the node

        // This algorithm just wants to get the higher possible
        auto bestNode = m_currentNode;
        uint maxEvaluation = m_lua["evaluate"](m_currentNode->altitude);
        for (const auto& neighbour : m_currentNode->neighbours) {
            // Get the evaluation from lua
            uint evaluation = m_lua["evaluate"](neighbour->altitude);
            if (evaluation > maxEvaluation) {
                bestNode = neighbour;
                maxEvaluation = evaluation;
            }
        }

        // If there is none higher, pick one randomly
        if (m_currentNode == bestNode)
            bestNode = alea::rand(m_currentNode->neighbours);

        // Switch to new node
        m_currentNode = bestNode;
        refreshPositionFromNode();
    }
}

//-------------------------//
//----- Dungeon graph -----//

void Hero::useGraph(const Graph& graph)
{
    m_graph = &graph;
    m_currentNode = &m_graph->node({0u, 0u});
    // FIXME Be sure it exists, otherwise an exception is thrown...
    // Get the door from the graph.
    refreshPositionFromNode();
}

//-----------------------------------//
//----- Internal change updates -----//

void Hero::refreshPositionFromNode()
{
    setLocalPosition(m_inter->roomLocalPosition(m_currentNode->room) + m_inter->roomSize() / 2.f);
}
