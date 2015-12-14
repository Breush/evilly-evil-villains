#pragma once

#include <vector>

namespace ai
{
    // Forward declarations

    struct Node;

    //! The abstraction of a way to a celle in a graph.
    struct Neighbour
    {
        Node* node = nullptr;   //!< The cell to go.
        void* data = nullptr;   //!< Extra data pointer for the user.
    };

    //! The abstraction of a cell in a graph.
    struct Node
    {
        std::vector<Neighbour> neighbours;  //!< The neighbourhood of the node.
        void* data = nullptr;               //!< Extra data pointer for the user.
    };
}
