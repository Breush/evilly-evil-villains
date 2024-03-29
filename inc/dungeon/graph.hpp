#pragma once

#include "ai/graph.hpp"
#include "tools/vector.hpp"
#include "context/event.hpp"
#include "dungeon/structs/room.hpp"

namespace dungeon
{
    // Forward declarations

    class Data;
    class FacilityInfo;

    //! The abstract structure of a dungeon.

    class Graph final : public ai::Graph, public context::EventReceiver
    {
    public:

        //! The abstraction of a passage between two dungeon rooms.
        struct NeighbourData
        {
            std::wstring tunnelFacilityID;  //!< The ID of the facility providing this way (empty if not a tunnel way).
        };

        //! The abstraction of a room in a dungeon.
        /*!
         *  It only sees which rooms are accessible from a certain point.
         *  It also contains different weights for different elements of the room.
         */
        struct NodeData
        {
            ai::Node* node = nullptr;   //!< Reference to the node.

            RoomCoords coords;          //!< The original room coordinates from data.
            bool constructed = false;   //!< True if node is a constructed room.

            uint altitude = 0u;         //!< How high is the node.
            uint treasure = 0u;         //!< How many money there is stored in the node (total).
            bool entrance = false;      //!< Whether the node is an entrance or not.

            std::vector<std::unique_ptr<NeighbourData>> neighbours; //!< Extra info about its neighbourhood.
        };

    public:

        //! Default constructor.
        Graph() = default;

        //! Default destructor.
        ~Graph() = default;

        //---------------------//
        //! @name Dungeon data
        //! @{

        //! The data of the dungeon to be read from.
        void useData(Data& data);

        //! @}

        //--------------------//
        //! @name Interaction
        //! @{

        //! Simple getter to access nodes.
        const ai::Node* node(const RoomCoords& coords) const;

        //! Simple getter to access nodes data.
        const NodeData* nodeData(const RoomCoords& coords) const;

        //! @}

    protected:

        //---------------//
        //! @name Events
        //! @{

        void receive(const context::Event& event) final;

        //! @}

        //---------------------------//
        //! @name Graph construction
        //! @{

        //! Add a neighbour to a node.
        void addNodeNeighbour(NodeData& nodeData, const RoomCoords& neighbourCoords, const std::wstring& tunnelFacilityID = L"");

        //! @}

        //--------------------------------//
        //! @name Internal changes update
        //! @{

        //! Refresh the treasure dosh of a node.
        void refreshTreasure(NodeData& node);

        //! Reconstruct size of graph (and update it) to the current data.
        void reconstructFromData();

        //! Updates the graph to the current data.
        void updateFromData();

        //! @}

    private:

        //! The data of the dungeon to be read from.
        Data* m_data = nullptr;

        //! The references to nodes, convert coords to nodes.
        std::vector<std::vector<NodeData>> m_nodes;

        uint8 m_floorsCount = 0u;   //!< Number of nodes.
        uint8 m_floorRoomsCount = 0u;  //!< Number of nodes.
    };
}
