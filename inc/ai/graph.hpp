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

        //--------------//
        //! @name Graph
        //! @{

        //! Add a node to the graph.
        Node& addNode(void* data = nullptr);

        //! Set the starting node.
        void setStartingNode(Node* startingNode);

        //! @}

        //--------------------//
        //! @name Interaction
        //! @{

        //! Simple getter to access the starting node.
        inline const Node* startingNode() const { return m_startingNode; }

        //! How many different nodes compose the graph.
        inline uint uniqueNodesCount() const { return m_nodes.size(); }

        //! @}

    protected:

        //--------------//
        //! @name Graph
        //! @{

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
        Node* m_startingNode = nullptr;
    };
}
