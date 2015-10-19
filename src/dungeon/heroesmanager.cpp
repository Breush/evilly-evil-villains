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

    returnif (!m_active);

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
    sf::Vector2u coords;

    if (event.type == "mode_changed") {
        setActive(devent.mode == Mode::INVASION);
    }
    else if (event.type == "room_exploded") {
        // Remove all heroes in that room
        for (auto& heroInfo : m_heroesInfo) {
            auto& hero = heroInfo.hero;
            if (hero == nullptr) continue;

            coords = {devent.action.room.x, devent.action.room.y};
            if (m_inter.tileFromLocalPosition(hero->localPosition()) == coords
                || reinterpret_cast<const Graph::NodeData*>(hero->currentNode()->data)->coords == coords) {
                heroInfo.status = HeroStatus::TO_BE_REMOVED;
            }
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
}

//-----------------------//
//----- Interaction -----//

bool HeroesManager::isHeroNearby(const sf::Vector2f& position, float range) const
{
    // Range squared
    range *= range;

    for (const auto& heroInfo : m_heroesInfo)
    if (heroInfo.hero != nullptr) {
        auto distance = position - heroInfo.hero->localPosition();
        distance *= distance;

        if ((distance.x + distance.y) < range)
            return true;
    }

    return false;
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

void HeroesManager::setActive(bool inActive)
{
    returnif (m_graph == nullptr || m_data == nullptr);
    returnif (m_active == inActive);
    m_active = inActive;

    // Start first wave of heroes
    if (m_active) {
        spawnHeroesGroup();
    }

    // No more running, all heroes escaped
    else {
        std::erase_if(m_heroesInfo, [](const HeroInfo& heroInfo) { return heroInfo.hero == nullptr; });
        for (auto& heroInfo : m_heroesInfo)
            heroGetsOut(heroInfo.hero.get());
    }
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

void HeroesManager::heroLeftRoom(Hero* hero, const sf::Vector2u& coords)
{
    Event devent;
    devent.type = "hero_left";
    devent.action.room = {coords.x, coords.y};
    devent.action.hero = hero;
    emitter()->emit(devent);
}

void HeroesManager::heroEnteredRoom(Hero* hero, const sf::Vector2u& coords)
{
    Event devent;
    devent.type = "hero_entered";
    devent.action.room = {coords.x, coords.y};
    devent.action.hero = hero;
    emitter()->emit(devent);
}

void HeroesManager::heroStealsTreasure(Hero* hero, const sf::Vector2u& coords, const uint stolenDosh)
{
    m_data->stealTreasure(coords, *hero, stolenDosh);
}
