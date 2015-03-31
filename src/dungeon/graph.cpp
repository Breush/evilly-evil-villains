#include "dungeon/graph.hpp"

#include "dungeon/data.hpp"

#include <stdexcept> // runtime_error

using namespace dungeon;

//------------------------//
//----- Dungeon data -----//

void Graph::useData(Data& data)
{
    m_data = &data;
}

bool Graph::reconstructFromData()
{
    if (m_data == nullptr)
        throw std::runtime_error("Reconstructing graph with not set dungeon data.");

    reset();

    const auto& floorsCount = m_data->floorsCount();
    const auto& roomsByFloor = m_data->roomsByFloor();

    // TODO Make a test of this functionality!
    // TODO Manage graph construction errors (return a string, keep empty if ok):
    //  - more than one door
    //  - no door
    //  - no treasure rooms
    //  (No access to treasure room is not an error, just a bad strategy for the player.)

    for (uint floorIndex = 0u; floorIndex < floorsCount; ++floorIndex)
    for (uint roomIndex = 0u; roomIndex < roomsByFloor; ++roomIndex) {
        sf::Vector2u roomCoords({floorIndex, roomIndex});
        auto& room = m_data->room(roomCoords);

        if (m_data->isRoomConstructed(roomCoords)) {
            // Initialize this particular room
            auto& node = m_nodes[roomCoords];
            node.altitude = floorIndex + 1u;
            node.room = roomCoords;
            if (room.facilities.door) m_startingNode = &node;

            // Check neighbourhood
            for (auto direction : {Data::EAST, Data::WEST, Data::NORTH, Data::SOUTH})
                if (m_data->roomNeighbourAccessible(roomCoords, direction))
                    node.neighbours.emplace_back(&m_nodes[m_data->roomNeighbourCoords(roomCoords, direction)]);
        }
    }

    return true;
}

//-----------------//
//----- Graph -----//

void Graph::reset()
{
    m_nodes.clear();
    m_startingNode = nullptr;
}
