#include "dungeon/heroesmanager.hpp"

#include "dungeon/inter.hpp"
#include "tools/random.hpp"

using namespace dungeon;

HeroesManager::HeroesManager()
{
}

//-------------------//
//----- Routine -----//

void HeroesManager::update(const sf::Time& dt)
{
    // Remove dead/out heroes
    std::erase_if(m_heroesInfo, [](const HeroInfo& heroInfo) { return heroInfo.status == HeroStatus::TO_BE_REMOVED; });

    returnif (m_graph == nullptr);

    // Consider other updates
    for (auto& heroInfo : m_heroesInfo) {
        // Spawn hero or update delay before spawning
        if (heroInfo.status == HeroStatus::TO_SPAWN) {
            heroInfo.spawnDelay -= dt.asSeconds();
            if (heroInfo.spawnDelay <= 0.f) {
                heroInfo.status = HeroStatus::RUNNING;
                auto& hero = heroInfo.hero;
                hero = std::make_unique<Hero>(*this, *m_inter, *m_graph);
                hero->bindElementData(heroInfo.data);
                m_inter->attachChild(*hero);
            }
        }
    }

    // Next group
    m_nextGroupDelay -= dt.asSeconds();
    if (m_nextGroupDelay <= 0.f)
        spawnHeroesGroup();
}

//------------------//
//----- Events -----//

void HeroesManager::receive(const context::Event& event)
{
    const auto& devent = *reinterpret_cast<const dungeon::Event*>(&event);

    if (devent.type == "room_destroyed") {
        // Remove all heroes in that room
        // FIXME Ultimately, this should be part of dungeon::Data
        // (Well, all that HeroesManager should)
        for (auto& heroInfo : m_heroesInfo) {
            auto& hero = heroInfo.hero;
            if (hero == nullptr) continue;

            sf::Vector2u coords = {devent.room.x, devent.room.y};
            if (m_inter->tileFromLocalPosition(hero->localPosition()) == coords)
                heroInfo.status = HeroStatus::TO_BE_REMOVED;
        }
    }
    else if (devent.type == "dungeon_graph_changed") {
        // TODO Should be a refreshHeroes() ICU
        for (auto& heroInfo : m_heroesInfo) {
            if (heroInfo.status == HeroStatus::RUNNING)
                heroInfo.hero->bindElementData(heroInfo.data);
        }
    }
}

//----------------------------//
//----- File interaction -----//

void HeroesManager::load(const pugi::xml_node& node)
{
    // TODO
}

void HeroesManager::save(pugi::xml_node node)
{
    // TODO
}

//-------------------------------//
//----- Dungeon interaction -----//

void HeroesManager::useInter(Inter& inter)
{
    m_inter = &inter;
    m_data = &m_inter->data();
    setEmitter(m_data);
}

void HeroesManager::useGraph(Graph& graph)
{
    m_graph = &graph;
    spawnHeroesGroup();
}

//-----------------------------------//
//----- Artificial intelligence -----//

const Graph::NodeData* HeroesManager::toNodeData(const ai::Node* node)
{
    returnif (node == nullptr) nullptr;
    return reinterpret_cast<const Graph::NodeData*>(node->data);
}

void HeroesManager::spawnHeroesGroup()
{
    const auto& startingNodes = m_graph->startingNodes();
    returnif (startingNodes.empty());

    uint newHeroesCount = 1u + rand() % 5u;

    float delay = 0.f;
    for (uint i = 0u; i < newHeroesCount; ++i) {
        m_heroesInfo.emplace_back();
        auto& heroInfo = m_heroesInfo.back();

        // FIXME We're spawning only Gr'oo right now...
        // We should make a more complex group theory
        heroInfo.data.create(L"groo");
        heroInfo.data[L"dosh"].init_uint32(0u);
        heroInfo.spawnDelay = delay;

        // Choose an entrance
        auto startingNode = alea::rand(startingNodes);
        auto coords = toNodeData(startingNode)->coords;
        heroInfo.data[L"rx"].init_float(coords.x + 0.5f);
        heroInfo.data[L"ry"].init_float(coords.y + 0.5f);

        delay += 3.f + static_cast<float>(rand() % 20u);
    }

    m_nextGroupDelay = 37.f + static_cast<float>(rand() % 120u);
}

//---------------------------//
//----- Heroes feedback -----//

void HeroesManager::heroGetsOut(Hero* hero)
{
    // If no dosh stolen, hero is unhappy, fame decrease
    if (hero->edata().at(L"dosh").as_uint32() == 0u)
        m_data->subFame(4u);
    else
        m_data->addFame(1u);

    // Remove hero from list
    hero->setVisible(false);
    for (auto& heroInfo : m_heroesInfo) {
        if (heroInfo.hero.get() == hero) {
            heroInfo.status = HeroStatus::TO_BE_REMOVED;
            break;
        }
    }
}

void HeroesManager::heroStealsTreasure(Hero* hero, const sf::Vector2u& coords, const uint stolenDosh)
{
    m_data->stealTreasure(coords, *hero, stolenDosh);
}
