#include "ai/graph.hpp"
#include "ai/node.hpp"

using namespace ai;

//-----------------//
//----- Graph -----//

Node& Graph::addNode(void* data)
{
    m_nodes.emplace_back();
    m_nodes.back().data = data;
    return m_nodes.back();
}

void Graph::setStartingNode(Node* startingNode)
{
    m_startingNode = startingNode;
}

void Graph::reset(uint nodesCount)
{
    m_nodes.clear();
    m_nodes.reserve(nodesCount);
    m_startingNode = nullptr;
}
