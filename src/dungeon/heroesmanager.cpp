#include "dungeon/heroesmanager.hpp"

#include "dungeon/inter.hpp"

using namespace dungeon;

HeroesManager::HeroesManager(Inter& inter)
    : m_inter(inter)
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
            heroInfo.data -= dt.asSeconds();
            if (heroInfo.data <= 0.f) {
                auto& hero = heroInfo.hero;
                hero = std::make_unique<Hero>(*this, m_inter);
                hero->setLocalScale(m_inter.roomScale());
                hero->useGraph(*m_graph);
                m_inter.attachChild(*hero);
                heroInfo.status = HeroStatus::RUNNING;
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
            if (m_inter.tileFromLocalPosition(hero->localPosition()) == coords)
                heroInfo.status = HeroStatus::TO_BE_REMOVED;
        }
    }
    else if (devent.type == "dungeon_graph_changed") {
        for (auto& heroInfo : m_heroesInfo) {
            // TODO Do something cleverer
            if (heroInfo.status == HeroStatus::RUNNING)
                heroInfo.hero->useGraph(*m_graph);
        }
    }
}

//-------------------------------//
//----- Dungeon interaction -----//

void HeroesManager::useData(Data& data)
{
    m_data = &data;
    setEmitter(m_data);
}

void HeroesManager::useGraph(Graph& graph)
{
    m_graph = &graph;
    spawnHeroesGroup();
}

//--------------------//
//----- Activity -----//

void HeroesManager::spawnHeroesGroup()
{
    uint newHeroesCount = 1u + rand() % 5u;

    float delay = 0.f;
    for (uint i = 0u; i < newHeroesCount; ++i) {
        m_heroesInfo.emplace_back();
        m_heroesInfo.back().data = delay;
        delay += 2.f + static_cast<float>(rand() % 12u);
    }

    m_nextGroupDelay = 37.f + static_cast<float>(rand() % 120u);
}

//---------------------------//
//----- Heroes feedback -----//

void HeroesManager::heroGetsOut(Hero* hero)
{
    // If no dosh stolen, hero is unhappy, fame decrease
    if (hero->dosh() == 0u) m_data->subFame(4u);
    else                    m_data->addFame(1u);

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
