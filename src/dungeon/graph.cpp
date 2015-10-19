#include "dungeon/graph.hpp"

#include "ai/node.hpp"
#include "dungeon/data.hpp"
#include "tools/debug.hpp"

using namespace dungeon;

//-----------------------//
//----- Interaction -----//

const ai::Node& Graph::node(const sf::Vector2u& coords) const
{
    return *m_nodes.at(coords.x).at(coords.y).node;
}

//------------------//
//----- Events -----//

void Graph::receive(const context::Event& event)
{
    returnif (m_data == nullptr || m_nodes.empty());

    const auto& devent = *reinterpret_cast<const dungeon::Event*>(&event);
    if (event.type == "facility_changed")
        refreshTreasure(m_nodes.at(devent.room.x).at(devent.room.y));
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
    if (floorsCount != m_nodes.size() || roomsByFloor != m_nodes[0u].size()) {
        reset(floorsCount * roomsByFloor);
        m_nodes.resize(floorsCount);
        for (auto& floorNodes : m_nodes)
            floorNodes.resize(roomsByFloor);
    }

    // Affect invariable attributes
    for (uint floorIndex = 0u; floorIndex < floorsCount; ++floorIndex)
    for (uint roomIndex = 0u; roomIndex < roomsByFloor; ++roomIndex) {
        auto& nodeData = m_nodes.at(floorIndex).at(roomIndex);
        nodeData.coords = {floorIndex, roomIndex};
        nodeData.altitude = floorIndex + 1u;
        nodeData.node = &addNode(&nodeData);
    }

    return updateFromData();
}

Graph::ConstructError Graph::updateFromData()
{
    setStartingNode(nullptr);

    for (auto& floorNodes : m_nodes)
    for (auto& nodeData : floorNodes) {
        auto& node = *nodeData.node;

        // Cleaning
        node.neighbours.clear();
        nodeData.entrance = false;
        nodeData.constructed = m_data->isRoomConstructed(nodeData.coords);

        if (!nodeData.constructed) continue;

        auto& room = m_data->room(nodeData.coords);

        // Check facilities
        for (auto facilityData : room.facilities) {
            // Entrance
            if (facilityData.type() == L"entrance") {
                nodeData.entrance = true;
                if (startingNode() == nullptr) setStartingNode(&node);
                else return ConstructError::TOO_MANY_DOORS;
            }

            // Treasure
            else if (facilityData.type() == L"treasure") {
                refreshTreasure(nodeData);
            }
        }

        // Check neighbourhood
        for (auto direction : {Data::EAST, Data::WEST, Data::NORTH, Data::SOUTH}) {
            if (m_data->roomNeighbourAccessible(nodeData.coords, direction)) {
                auto neighbourCoords = m_data->roomNeighbourCoords(nodeData.coords, direction);
                node.neighbours.emplace_back(m_nodes.at(neighbourCoords.x).at(neighbourCoords.y).node);
            }
        }
    }

    // Construction errors
    if (startingNode() == nullptr)
        return ConstructError::NO_DOOR;

    return ConstructError::NONE;
}

//-----------------------------------//
//----- Internal changes update -----//

void Graph::refreshTreasure(NodeData& nodeData)
{
    nodeData.treasure = 0u;

    returnif (!m_data->isRoomConstructed(nodeData.coords));

    // TODO Should be a getTreasure() function in all facilities
    auto& room = m_data->room(nodeData.coords);
    for (auto facilityData : room.facilities)
        if (facilityData.type() == L"treasure")
            nodeData.treasure += facilityData[L"dosh"].as_uint32();
}
