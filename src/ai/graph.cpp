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

void Graph::addStartingNode(Node* startingNode)
{
    m_startingNodes.emplace_back(startingNode);
}

void Graph::resetStartingNodes()
{
    m_startingNodes.clear();
}

void Graph::reset(uint nodesCount)
{
    m_nodes.clear();
    m_startingNodes.clear();
    m_nodes.reserve(nodesCount);
}
