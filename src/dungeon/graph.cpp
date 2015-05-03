#include "dungeon/graph.hpp"

#include "dungeon/data.hpp"
#include "tools/debug.hpp"

using namespace dungeon;

//------------------------//
//----- Dungeon data -----//

void Graph::useData(Data& data)
{
    m_data = &data;
    m_data->linkGraph(this);
}

Graph::ConstructError Graph::reconstructFromData()
{
    massert(m_data != nullptr, "Reconstructing dungeon::Graph with dungeon::Data not set.");
    reset();

    const auto& floorsCount = m_data->floorsCount();
    const auto& roomsByFloor = m_data->roomsByFloor();

    for (uint floorIndex = 0u; floorIndex < floorsCount; ++floorIndex)
    for (uint roomIndex = 0u; roomIndex < roomsByFloor; ++roomIndex) {
        sf::Vector2u roomCoords({floorIndex, roomIndex});
        auto& room = m_data->room(roomCoords);

        if (m_data->isRoomConstructed(roomCoords)) {
            // Initialize this particular room
            auto& node = m_nodes[roomCoords];
            node.altitude = floorIndex + 1u;
            node.coords = roomCoords;

            // Entrance
            if (room.facilities[FacilityID::ENTRANCE]) {
                node.entrance = true;
                if (m_startingNode == nullptr) m_startingNode = &node;
                else return ConstructError::TOO_MANY_DOORS;
            }
            // Treasure
            if (room.facilities[FacilityID::TREASURE]) {
                node.treasure = room.treasureDosh;
            }

            // Check neighbourhood
            for (auto direction : {Data::EAST, Data::WEST, Data::NORTH, Data::SOUTH})
                if (m_data->roomNeighbourAccessible(roomCoords, direction))
                    node.neighbours.emplace_back(&m_nodes[m_data->roomNeighbourCoords(roomCoords, direction)]);
        }
    }

    // Construction errors
    if (m_startingNode == nullptr)
        return ConstructError::NO_DOOR;

    return ConstructError::NONE;
}

//-----------------//
//----- Graph -----//

void Graph::reset()
{
    m_nodes.clear();
    m_startingNode = nullptr;
}
