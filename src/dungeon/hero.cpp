#include "dungeon/hero.hpp"

#include "resources/identifiers.hpp"
#include "dungeon/inter.hpp"
#include "dungeon/data.hpp"
#include "tools/random.hpp"
#include "tools/debug.hpp"
#include "tools/tools.hpp"

#include <stdexcept> // runtime_error

using namespace dungeon;

Hero::Hero(const Inter* inter)
    : baseClass(true)
    , m_running(false)
    , m_dosh(0u)
    , m_inter(inter)
{
    setVisible(false);

    lerpable()->setPositionSpeed({50.f, 25.f});

    // Dosh label
    // TODO Should be in NUI layer, somehow
    m_doshLabel.setPrestyle(sfe::Label::Prestyle::NUI);
    m_doshLabel.setLocalPosition({5.f, -25.f});
    m_doshLabel.centerOrigin();
    attachChild(m_doshLabel);

    // Sprite
    attachChild(m_sprite);
    m_sprite.load(AnimationID::HEROES_GROO);
    // TODO Should not exists...
    m_sprite.setLocalScale({0.15f, 0.15f});

    // Lua
    if (!m_lua.load("res/ai/hero.lua"))
        throw std::runtime_error("Failed to load hero AI.");

    // Register
    m_lua["AIGetOut"] = [this] { AIGetOut(); };
    m_lua["AIStealTreasure"] = [this] { AIStealTreasure(); };
}

//-------------------//
//----- Routine -----//

void Hero::updateAI(const sf::Time& dt)
{
    returnif (!m_running);

    // Look for next room
    returnif (lerpable()->positionLerping());

    returnif (m_currentNode == nullptr);
    returnif (m_currentNode->neighbours.size() == 0u);

    // Consider that the current room might be the best node
    std::vector<const Graph::Node*> bestNodes;
    bestNodes.push_back(m_currentNode);
    int maxEvaluation = call("evaluate_reference", m_currentNode);

    // If the hero escaped during evaluation, stop it here
    returnif (m_currentNode == nullptr);

    // Get the evaluation from lua
    for (const auto& neighbour : m_currentNode->neighbours) {
        int evaluation = call("evaluate", neighbour);

        // Found a new limit for the best nodes
        if (evaluation > maxEvaluation) {
            maxEvaluation = evaluation;
            bestNodes.clear();
        }

        // This node is among the best ones
        if (evaluation == maxEvaluation)
            bestNodes.push_back(neighbour);
    }

    // Switch to a new node randomly from the best ones
    setCurrentNode(alea::rand(bestNodes));
    ++m_tick;
}

void Hero::setCurrentNode(const Graph::Node* node, bool firstNode)
{
    returnif (m_currentNode == node);

    Event event;
    event.action.hero = this;

    // Emit signal when getting out
    if (m_currentNode != nullptr) {
        event.type = EventType::HERO_LEFT_ROOM;
        event.action.room = {m_currentNode->coords.x, m_currentNode->coords.y};
        emitter()->emit(event);
    }

    m_currentNode = node;

    if (m_currentNode != nullptr) {
        // First visit to this node
        if (m_nodeInfos[m_currentNode->coords].visits == 0u)
            m_lua["nonVisitedNodes"] = static_cast<uint>(m_lua["nonVisitedNodes"]) - 1u;

        m_nodeInfos[m_currentNode->coords].visits += 1u;
        m_nodeInfos[m_currentNode->coords].lastVisit = m_tick;
        refreshPositionFromNode(firstNode);

        event.type = EventType::HERO_ENTERED_ROOM;
        event.action.room = {m_currentNode->coords.x, m_currentNode->coords.y};
        emitter()->emit(event);
    }
}

//-----------------------------------//
//----- Artificial intelligence -----//

uint Hero::call(const char* function, const Graph::Node* node)
{
    Weight weight;
    weight.visited =    m_nodeInfos[node->coords].visits;
    weight.lastVisit =  m_nodeInfos[node->coords].lastVisit;
    weight.altitude =   node->altitude;
    weight.treasure =   node->treasure;
    weight.exit =       node->entrance;

    m_lua["weight"].SetObj(weight,
                           "visited",   &Weight::visited,
                           "lastVisit", &Weight::lastVisit,
                           "altitude",  &Weight::altitude,
                           "treasure",  &Weight::treasure,
                           "exit",      &Weight::exit);

    return m_lua[function]();
}

void Hero::AIGetOut()
{
    returnif (!m_currentNode->entrance);
    setRunning(false);

    // If no dosh stolen -> hero is unhappy, fame decrease
    if (dosh() == 0u) {
        m_data->subFame(4u);
    }

    // The dosh held is lost for the player.
    else {
        setDosh(0u);
        m_data->addFame(1u);
    }
}

void Hero::AIStealTreasure()
{
    auto maxStolenDosh = std::min(100u, m_currentNode->treasure);
    auto stolenDosh = 1u + rand() % maxStolenDosh;
    m_data->stealTreasure(m_currentNode->coords, *this, stolenDosh);
}

//-------------------------//
//----- Dungeon graph -----//

void Hero::useGraph(Graph& graph)
{
    m_graph = &graph;
}

//--------------------------//
//----- Dungeon events -----//

void Hero::useData(Data& data)
{
    m_data = &data;
    setEmitter(&data);
}

void Hero::receive(const Event& event)
{
    returnif (event.type != EventType::MODE_CHANGED);
    setRunning(event.mode == Mode::INVASION);
}

//-----------------------------------//
//----- Internal change updates -----//

void Hero::changedRunning()
{
    setVisible(m_running);

    if (m_running) {
        // Reinitialize AI
        m_tick = 0u;
        m_nodeInfos.clear();
        m_lua["init"]();

        // Get the door from the graph (requires that it is correctly constructed).
        m_lua["nonVisitedNodes"] = m_graph->uniqueNodesCount();
        setCurrentNode(&m_graph->startingNode(), true);
    }
    else {
        setCurrentNode(nullptr);
    }
}

void Hero::changedDosh()
{
    // Remove all text if no dosh held
    if (m_dosh == 0u) {
        m_doshLabel.setText(L"");
        return;
    }

    // Otherwise, write the correct text
    std::wstringstream str;
    str << m_dosh << "d";
    m_doshLabel.setText(str.str());
}

void Hero::refreshPositionFromNode(bool firstNode)
{
    const auto heroTilePosition = sf::Vector2f{m_inter->tileSize().x / 2.f, m_inter->tileSize().y * 0.9f};
    lerpable()->setTargetPosition(m_inter->tileLocalPosition(m_currentNode->coords) + heroTilePosition);
    if (firstNode) setLocalPosition(lerpable()->targetPosition());

    // Select correct animation (right/left)
    // TODO Have a state machine and interface with physics module
    // Maybe the state machine animated sprite makes its own class
    if (lerpable()->targetPosition().x > localPosition().x)
        m_sprite.select(L"rwalk");
    else if (lerpable()->targetPosition().x < localPosition().x)
        m_sprite.select(L"lwalk");
}
