#include "dungeon/data.hpp"
#include "dungeon/graph.hpp"

#include <iostream>

int main(void)
{
    dungeon::Data data;
    dungeon::Graph graph;

    // Loading and initializing
    data.load(L"../tests/data/test-graph/");
    graph.useData(data);
    graph.reconstructFromData();

    const auto& node = graph.startingNode();

    // Starting node
    sf::Vector2u expectedDoorRoom(0u, 2u);
    if (node.room != expectedDoorRoom) {
        std::cerr << "Starting room is not set correctly." << std::endl;
        std::cerr << "Found: " << node.room << " | Expected: " << expectedDoorRoom << std::endl;
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
