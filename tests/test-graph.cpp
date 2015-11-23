#include "dungeon/data.hpp"
#include "dungeon/graph.hpp"

#include <iostream>

const dungeon::Graph::NodeData* toNodeData(const ai::Node* node)
{
    returnif (node == nullptr) nullptr;
    return reinterpret_cast<const dungeon::Graph::NodeData*>(node->data);
}

int main(void)
{
    dungeon::Data data;
    dungeon::Graph graph;

    // Loading and initializing
    data.load(L"../tests/data/test-graph/");
    graph.useData(data);

    const auto& node = *graph.startingNodes()[0];
    const auto& nodeData = *toNodeData(&node);

    // Starting node
    sf::Vector2u expectedEntranceCoords(0u, 2u);
    if (nodeData.coords != expectedEntranceCoords) {
        std::cerr << "Starting room is not set correctly." << std::endl;
        std::cerr << "Found: " << nodeData.coords << " | Expected: " << expectedEntranceCoords << std::endl;
        return EXIT_FAILURE;
    }

    // Neighbours
    uint expectedNeighbours(2u);
    if (node.neighbours.size() != expectedNeighbours) {
        std::cerr << "Wrong number of neighbours for starting node: " << std::endl;
        std::cerr << "Found: " << node.neighbours.size() << " | Expected: " << expectedNeighbours << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
