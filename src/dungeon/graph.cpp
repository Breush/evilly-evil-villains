#include "dungeon/graph.hpp"

#include "dungeon/data.hpp"
#include "tools/debug.hpp"

using namespace dungeon;

//------------------//
//----- Events -----//

void Graph::receive(const context::Event& event)
{
    returnif (m_data == nullptr || m_nodes.empty());

    const auto& devent = *reinterpret_cast<const dungeon::Event*>(&event);
    if (event.type == "facility_changed")
        refreshTreasure(m_nodes[devent.room.x][devent.room.y]);
}

//------------------------//
//----- Dungeon data -----//

void Graph::useData(Data& data)
{
    m_data = &data;
    m_data->linkGraph(this);
    setEmitter(m_data);
}

Graph::ConstructError Graph::reconstructFromData()
{
    massert(m_data != nullptr, "Reconstructing dungeon::Graph with dungeon::Data not set.");

    const auto& floorsCount = m_data->floorsCount();
    const auto& roomsByFloor = m_data->roomsByFloor();

    // Soft reset: will keep memory as it was if same dungeon size.
    m_startingNode = nullptr;
    if (floorsCount != m_nodes.size() || roomsByFloor != m_nodes[0u].size()) {
        m_nodes.resize(floorsCount);
        for (auto& floorNodes : m_nodes)
            floorNodes.resize(roomsByFloor);
    }

    // Affect invariable attributes
    for (uint floorIndex = 0u; floorIndex < floorsCount; ++floorIndex)
    for (uint roomIndex = 0u; roomIndex < roomsByFloor; ++roomIndex) {
        sf::Vector2u roomCoords({floorIndex, roomIndex});
        auto& node = m_nodes[floorIndex][roomIndex];
        node.altitude = floorIndex + 1u;
        node.coords = roomCoords;
    }

    return updateFromData();
}

Graph::ConstructError Graph::updateFromData()
{
    m_startingNode = nullptr;

    for (auto& floorNodes : m_nodes)
    for (auto& node : floorNodes) {
        // Cleaning
        node.entrance = false;
        node.neighbours.clear();
        node.constructed = m_data->isRoomConstructed(node.coords);

        if (!node.constructed) continue;

        auto& room = m_data->room(node.coords);

        // Check facilities
        for (auto facilityData : room.facilities) {
            // Entrance
            if (facilityData.type() == L"entrance") {
                node.entrance = true;
                if (m_startingNode == nullptr) m_startingNode = &node;
                else return ConstructError::TOO_MANY_DOORS;
            }

            // Treasure
            else if (facilityData.type() == L"treasure") {
                refreshTreasure(node);
            }
        }

        // Check neighbourhood
        for (auto direction : {Data::EAST, Data::WEST, Data::NORTH, Data::SOUTH}) {
            if (m_data->roomNeighbourAccessible(node.coords, direction)) {
                auto neighbourCoords = m_data->roomNeighbourCoords(node.coords, direction);
                node.neighbours.emplace_back(&m_nodes[neighbourCoords.x][neighbourCoords.y]);
            }
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

//-----------------------------------//
//----- Internal changes update -----//

void Graph::refreshTreasure(Node& node)
{
    node.treasure = 0u;

    returnif (!m_data->isRoomConstructed(node.coords));

    auto& room = m_data->room(node.coords);
    for (auto facilityData : room.facilities)
        if (facilityData.type() == L"treasure")
            node.treasure += facilityData[L"dosh"].as_uint32();
}
