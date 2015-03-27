#include "dungeon/hero.hpp"

#include "dungeon/inter.hpp"
#include "tools/random.hpp"
#include "tools/debug.hpp"

#include <selene/selene.hpp>

using namespace dungeon;

Hero::Hero(const Inter* inter)
    : m_inter(inter)
{
    // Sprite
    m_sprite.setSize({10.f, 10.f});
    m_sprite.setFillColor(sf::Color::White);
    addPart(&m_sprite);
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

        if (m_currentNode->neighbours.size() == 0u) {
            std::cerr << "No neighbours" << std::endl;
            return;
        }

        // This algorithm just wants to get the higher possible
        const Graph::Node* bestNode = m_currentNode;
        for (const auto& neighbour : m_currentNode->neighbours)
            if (neighbour->altitude > bestNode->altitude)
                bestNode = neighbour;

        // If there is none higher, pick one randomly
        if (m_currentNode == bestNode)
            bestNode = alea::rand(m_currentNode->neighbours);

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
