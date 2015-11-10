#pragma once

#include "tools/int.hpp"
#include "ai/node.hpp"

#include <vector>

namespace ai
{
    //! The abstract structure.

    class Graph
    {
    public:

        //! Constructor.
        Graph() = default;

        //! Default destructor.
        ~Graph() = default;

        //--------------------//
        //! @name Interaction
        //! @{

        //! Simple getter to access the starting node.
        inline const std::vector<Node*>& startingNodes() const { return m_startingNodes; }

        //! How many different nodes compose the graph.
        inline uint uniqueNodesCount() const { return m_nodes.size(); }

        //! @}

    protected:

        //--------------//
        //! @name Graph
        //! @{

        //! Add a node to the graph.
        Node& addNode(void* data = nullptr);

        //! Add a node to the starting list.
        void addStartingNode(Node* startingNode);

        //! Remove all nodes from the starting list.
        void resetStartingNodes();

        //! Erase all information about the graph, reserving some space.
        /*!
         *  Indicate a sufficently large value here is important
         *  to be sure addNode() returned references are not invalidated
         *  by a std::vector resize of m_nodes.
         */
        void reset(uint nodesCount);

        //! @}

    private:

        //! All the nodes within the graph.
        std::vector<Node> m_nodes;

        //! The entry point to the graph.
        std::vector<Node*> m_startingNodes;
    };
}
