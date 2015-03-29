#include "dungeon/graph.hpp"

#include "dungeon/data.hpp"
#include "tools/debug.hpp"

using namespace dungeon;

//------------------------//
//----- Dungeon data -----//

void Graph::useData(Data& data)
{
    m_data = &data;
}

bool Graph::reconstructFromData()
{
    assert(m_data != nullptr);
    m_nodes.clear();

    const auto& floorsCount = m_data->floorsCount();
    const auto& roomsByFloor = m_data->roomsByFloor();

    // TODO Make a test of this functionality!

    for (uint floor = 0u; floor < floorsCount; ++floor)
    for (uint room = 0u; room < roomsByFloor; ++room) {
        sf::Vector2u roomCoords({floor, room});

        if (m_data->isRoomConstructed(roomCoords)) {
            // Initialize this particular room
            auto& node = m_nodes[roomCoords];
            node.altitude = floor + 1u;
            node.room = roomCoords;

            // Check neighbourhood
            for (auto direction : {Data::EAST, Data::WEST, Data::NORTH, Data::SOUTH})
                if (m_data->roomNeighbourAccessible(roomCoords, direction))
                    node.neighbours.emplace_back(&m_nodes[m_data->roomNeighbourCoords(roomCoords, direction)]);
        }
    }

    return true;
}
