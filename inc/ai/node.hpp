#pragma once

#include <vector>

namespace ai
{
    //! The abstraction of a cell in a graph.
    struct Node
    {
        std::vector<Node*> neighbours;  //!< The neighbourhood of the node.
        void* data = nullptr;           //!< Extra data pointer for the user.
    };
}
