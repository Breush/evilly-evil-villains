#include "dungeon/hero.hpp"

#include "dungeon/heroesmanager.hpp"
#include "dungeon/inter.hpp"
#include "dungeon/data.hpp"
#include "tools/tools.hpp"

#include <stdexcept> // runtime_error

using namespace dungeon;

Hero::Hero(HeroesManager& manager, Inter& inter)
    : baseClass(true)
    , m_dosh(0u)
    , m_manager(manager)
    , m_inter(inter)
{
    lerpable()->setPositionSpeed({50.f, 25.f});

    // Sprite
    attachChild(m_sprite);
    m_sprite.load("heroes/groo");

    // Dosh label
    attachChild(m_doshLabel);
    m_doshLabel.setPrestyle(scene::Label::Prestyle::NUI);
    m_doshLabel.setLocalPosition({5.f, -25.f});
    m_doshLabel.centerOrigin();

    // Lua
    m_luaActor.loadFile("res/ai/hero.lua");
    m_luaActor.state()["AIGetOut"] = [this] { AIGetOut(); };
    m_luaActor.state()["AIStealTreasure"] = [this] { AIStealTreasure(); };

    // Debug - show more info
    #if DEBUG_AI > 0
    sf::Color colorMain{119u, 158u, 203u, 150u};
    sf::Color colorSecondary{174u, 198u, 207u, 150u};
    for (uint i = 0u; i < m_overlays.size(); ++i) {
        m_inter.attachChild(m_overlays[i]);
        m_overlays[i].setFillColor((i == 0u)? colorMain : colorSecondary);
        m_overlays[i].setVisible(false);
        m_overlays[i].setDepth(1.f);

        m_inter.attachChild(m_overlayLabels[i]);
        m_overlayLabels[i].setPrestyle(scene::Label::Prestyle::NUI_SOBER);
        m_overlayLabels[i].setDepth(0.5f);
    }
    #endif
}

//-------------------//
//----- Routine -----//

void Hero::onTransformChanges()
{
    #if DEBUG_AI > 0
    for (uint i = 0u; i < m_overlays.size(); ++i)
        m_overlays[i].setSize(sf::Vector2f{256.f, 384.f} * localScale());
    #endif
}

void Hero::updateAI(const sf::Time& dt)
{
    // Get next room if not already moving
    returnif (lerpable()->positionLerping());
    setCurrentNode(m_luaActor.findNextNode(m_currentNode));
}

//---------------------------//
//----- Node management -----//

void Hero::setCurrentNode(const Graph::Node* node)
{
    returnif (m_currentNode == node);

    #if DEBUG_AI > 0
    // Show what make us decide for the next room
    refreshDebugOverlays();
    #endif

    // Emit signal when getting out
    if (m_currentNode != nullptr)
        m_manager.heroLeftRoom(this, m_currentNode->coords);

    bool firstNode = (m_currentNode == nullptr);
    m_currentNode = node;

    #if DEBUG_AI > 0
    // Hide all when quitting
    if (m_currentNode == nullptr)
        refreshDebugOverlays();
    #endif

    if (m_currentNode != nullptr) {
        refreshPositionFromNode(firstNode);
        m_manager.heroEnteredRoom(this, m_currentNode->coords);
    }
}

//-----------------------------------//
//----- Artificial intelligence -----//

void Hero::AIGetOut()
{
    returnif (!m_currentNode->entrance);
    m_manager.heroGetsOut(this);
}

void Hero::AIStealTreasure()
{
    auto maxStolenDosh = std::min(100u, m_currentNode->treasure);
    auto stolenDosh = 1u + rand() % maxStolenDosh;
    m_manager.heroStealsTreasure(this, m_currentNode->coords, stolenDosh);
}

//-------------------------//
//----- Dungeon graph -----//

void Hero::useGraph(Graph& graph)
{
    m_luaActor.useGraph(graph);
    setCurrentNode(&graph.startingNode());
}

//-----------------------------------//
//----- Internal change updates -----//

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
    returnif (m_currentNode == nullptr);

    const auto tileLocalPosition = m_inter.tileLocalPosition(m_currentNode->coords);
    const auto heroTilePosition = sf::Vector2f{m_inter.tileSize().x / 2.f, m_inter.tileSize().y * 0.8f};
    lerpable()->setTargetPosition(tileLocalPosition + heroTilePosition);
    if (firstNode) setLocalPosition(lerpable()->targetPosition());

    // Select correct animation (right/left)
    // TODO Have a state machine and interface with physics module
    // Maybe the state machine animated sprite makes its own class
    if (lerpable()->targetPosition().x > localPosition().x)
        m_sprite.select(L"rwalk");
    else if (lerpable()->targetPosition().x < localPosition().x)
        m_sprite.select(L"lwalk");
}

#if DEBUG_AI > 0
std::wstring stringFromWeight(const ai::LuaActor::Weight& weight, const int evaluation)
{
    std::wstringstream str;
    str << "eval.     " << evaluation << std::endl;
    str << std::endl;
    str << "visited   " << weight.visited << std::endl;
    str << "lastVisit " << weight.lastVisit << std::endl;
    str << std::endl;
    str << "altitude  " << weight.altitude << std::endl;
    str << "treasure  " << weight.treasure << std::endl;
    str << "exit      " << weight.exit;
    return std::move(str.str());
}

void Hero::refreshDebugOverlay(uint index, const Graph::Node* node)
{
    // Make it visible
    m_overlays[index].setVisible(true);
    m_overlayLabels[index].setVisible(true);

    // Reposition
    const auto position = m_inter.tileLocalPosition(node->coords);
    m_overlays[index].setLocalPosition(position);
    m_overlayLabels[index].setLocalPosition(position);

    // Text content
    m_overlayLabels[index].setText(stringFromWeight(m_luaActor.getWeight(node), m_luaActor.evaluation(index)));
}

void Hero::refreshDebugOverlays()
{
    // Hide all
    for (uint i = 0u; i < m_overlays.size(); ++i) {
        m_overlays[i].setVisible(false);
        m_overlayLabels[i].setVisible(false);
    }

    returnif (m_currentNode == nullptr);

    // Main
    refreshDebugOverlay(0u, m_currentNode);

    // Neighbours
    uint i = 0u;
    for (; i < m_currentNode->neighbours.size(); ++i)
        refreshDebugOverlay(i + 1u, m_currentNode->neighbours[i]);

    // Remove all not neighbour
    for (++i; i < m_overlays.size(); ++i) {
        m_overlays[i].setVisible(false);
        m_overlayLabels[i].setVisible(false);
    }
}
#endif
