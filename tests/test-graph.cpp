#include "dungeon/data.hpp"
#include "dungeon/graph.hpp"

#include <iostream>

int main(void)
{
    dungeon::Data data;
    dungeon::Graph graph;

    data.load(L"tests/test-graph-");
    graph.useData(data);
    graph.reconstructFromData();

    if (graph.startingNode().room != sf::Vector2u{0u, 2u}) {
        std::cerr << "Starting room is not set correctly." << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
