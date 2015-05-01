#pragma once

#include "tools/vector.hpp"

#include <vector>
#include <unordered_map>

namespace dungeon
{
    // Forward declarations

    class Data;

    //! The abstract structure of a dungeon.

    class Graph final
    {
    public:

        //! The type of error when constructing from data.
        enum ConstructError
        {
            NONE,           //! No error so far.
            NO_DOOR,        //! No door was found.
            TOO_MANY_DOORS, //! Too many doors were found.
        };

        //! The abstraction of a room in a dungeon.
        /*!
         *  It only sees which rooms are accessible from a certain point.
         *  It also contains different weights for different elements of the room.
         */
        struct Node
        {
            std::vector<Node*> neighbours;  //!< The neighbourhood of the node.
            sf::Vector2u room;              //!< The original room coordinates from data.

            uint altitude = 0u;             //!< How high is the node.
            uint treasure = 0u;             //!< How many money there is stored in the node (total).
            bool entrance = false;          //!< Whether the node is an entrance or not.
        };

    public:

        //! Constructor.
        Graph() = default;

        //! Default destructor.
        ~Graph() = default;

        //---------------------//
        //! @name Dungeon data
        //! @{

        //! The data of the dungeon to be read from.
        void useData(Data& data);

        //! Updates the graph to the current data.
        //! Returns the type of error if not well formatted.
        ConstructError reconstructFromData();

        //! @}

        //--------------------//
        //! @name Interaction
        //! @{

        //! Simple getter to access nodes.
        inline const Node& node(const sf::Vector2u& roomCoords) const { return m_nodes.at(roomCoords); }

        //! Simple getter to access the starting node.
        inline const Node& startingNode() const { return *m_startingNode; }

        //! @}

    protected:

        //--------------//
        //! @name Graph
        //! @{

        //! Erase all information about the graph (still keeping the data).
        void reset();

        //! @}

    private:

        //! The data of the dungeon to be read from.
        Data* m_data = nullptr;

        //! All the nodes within the graph.
        std::unordered_map<sf::Vector2u, Node> m_nodes;

        //! The node with the dungeon door.
        Node* m_startingNode = nullptr;
    };
}
