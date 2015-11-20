#include "dungeon/graph.hpp"

#include "ai/node.hpp"
#include "dungeon/data.hpp"
#include "tools/debug.hpp"

using namespace dungeon;

//-----------------------//
//----- Interaction -----//

const ai::Node* Graph::node(const sf::Vector2u& coords) const
{
    if (coords.x >= m_floorsCount || coords.y >= m_roomsByFloor)
        return nullptr;

    return m_nodes.at(coords.x).at(coords.y).node;
}

//------------------//
//----- Events -----//

void Graph::receive(const context::Event& event)
{
    returnif (m_data == nullptr || m_nodes.empty());

    const auto& devent = *reinterpret_cast<const dungeon::Event*>(&event);
    if (event.type == "facility_changed")
        refreshTreasure(m_nodes.at(devent.room.x).at(devent.room.y));
    else if (event.type == "dungeon_changed")
        reconstructFromData();
}

//------------------------//
//----- Dungeon data -----//

void Graph::useData(Data& data)
{
    m_data = &data;
    m_data->useGraph(*this);

    setEmitter(m_data);
    reconstructFromData();
}

void Graph::reconstructFromData()
{
    massert(m_data != nullptr, "Reconstructing dungeon::Graph with dungeon::Data not set.");

    m_floorsCount = m_data->floorsCount();
    m_roomsByFloor = m_data->roomsByFloor();

    // Soft reset: will keep memory as it was if same dungeon size.
    if (m_floorsCount != m_nodes.size() || m_roomsByFloor != m_nodes[0u].size()) {
        reset(m_floorsCount * m_roomsByFloor);
        m_nodes.resize(m_floorsCount);
        for (auto& floorNodes : m_nodes)
            floorNodes.resize(m_roomsByFloor);
    }

    // Affect invariable attributes
    for (uint floorIndex = 0u; floorIndex < m_floorsCount; ++floorIndex)
    for (uint roomIndex = 0u; roomIndex < m_roomsByFloor; ++roomIndex) {
        auto& nodeData = m_nodes.at(floorIndex).at(roomIndex);
        nodeData.coords = {floorIndex, roomIndex};
        nodeData.altitude = floorIndex + 1u;
        nodeData.node = &addNode(&nodeData);
    }

    updateFromData();
}

void Graph::updateFromData()
{
    resetStartingNodes();

    for (auto& floorNodes : m_nodes)
    for (auto& nodeData : floorNodes) {
        auto& node = *nodeData.node;

        // Cleaning
        node.neighbours.clear();
        nodeData.entrance = false;
        nodeData.constructed = m_data->isRoomConstructed(nodeData.coords);

        if (!nodeData.constructed)
            continue;

        const auto& coords = nodeData.coords;
        auto& room = m_data->room(coords);

        // Check facilities
        for (auto facilityInfo : room.facilities) {
            // Entrance
            if (facilityInfo.common->entrance) {
                nodeData.entrance = true;
                addStartingNode(&node);
            }

            // Treasure
            refreshTreasure(nodeData);

            // Tunnels
            for (const auto& tunnel : facilityInfo.tunnels) {
                sf::Vector2u tunnelCoords = sf::v2u(tunnel.coords);
                if (tunnel.relative) tunnelCoords += coords;

                if (m_data->isRoomConstructed(tunnelCoords))
                    node.neighbours.emplace_back(m_nodes.at(tunnelCoords.x).at(tunnelCoords.y).node);
            }
        }

        // Check neighbourhood
        for (auto direction : {EAST, WEST}) {
            auto neighbourCoords = m_data->roomNeighbourCoords(coords, direction);
            if (m_data->isRoomConstructed(neighbourCoords))
                node.neighbours.emplace_back(m_nodes.at(neighbourCoords.x).at(neighbourCoords.y).node);
        }
    }

    Event event;
    event.type = "dungeon_graph_changed";
    emitter()->emit(event);
}

//-----------------------------------//
//----- Internal changes update -----//

// FIXME BUG This refresh is called on facility_changed
// but the treasure entity has not been created (in Inter),
// so the treasure will not be refreshed unless an other event is
// launched from the Treasure constructor. (From Lua!)
// -> Make it react to a treasure_changed event.
void Graph::refreshTreasure(NodeData& nodeData)
{
    nodeData.treasure = 0u;

    returnif (!m_data->isRoomConstructed(nodeData.coords));

    auto& room = m_data->room(nodeData.coords);
    for (auto& facility : room.facilities) {
        const auto& treasure = facility.treasure;
        if (treasure != -1u)
            nodeData.treasure += treasure;
    }
}
