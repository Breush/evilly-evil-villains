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

        //! The abstraction of a room in a dungeon.
        /*!
         *  It only sees which rooms are accessible from a certain point.
         *  It also contains different weights for different elements of the room.
         */

        struct Node
        {
            std::vector<Node*> neighbours;      //!< The neighbourhood of the node.
            sf::Vector2u room;                  //!< The original room coordinates from data.

            //! All the weights used for algorithms.
            //! Note: this is a separate structure to be compatible with Lua.
            struct Weight
            {
                uint altitude = 0u;                 //!< How high is the node.
            } weight;
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
        //! Returns true if dungeon is well formatted.
        bool reconstructFromData();

        //! @}

        //--------------------//
        //! @name Interaction
        //! @{

        //! Simple getter to access nodes.
        inline const Node& node(const sf::Vector2u& roomCoords) const { return m_nodes.at(roomCoords); }

        //! @}

    private:

        //! The data of the dungeon to be read from.
        Data* m_data = nullptr;

        //! All the nodes within the graph.
        std::unordered_map<sf::Vector2u, Node> m_nodes;
    };
}
