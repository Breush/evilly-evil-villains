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
    for (const auto& hero : m_removeHeroes)
        std::erase_if(m_heroes, [hero](const std::unique_ptr<Hero>& inHero) { return inHero.get() == hero; });
    m_removeHeroes.clear();
}

//------------------//
//----- Events -----//

void HeroesManager::receive(const Event& event)
{
    returnif (event.type != EventType::MODE_CHANGED);
    setActive(event.mode == Mode::INVASION);
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

//--------------------//
//----- Activity -----//

void HeroesManager::setActive(bool inActive)
{
    returnif (m_graph == nullptr || m_data == nullptr);
    returnif (m_active == inActive);
    m_active = inActive;

    // Start first wave of heroes
    if (m_active) {
        // TODO Spawn more than one... random appearance + regularly
        auto hero = std::make_unique<Hero>(*this, m_inter);
        hero->setLocalScale(m_inter.roomScale());
        hero->useGraph(*m_graph);
        m_inter.attachChild(*hero);
        m_heroes.emplace_back(std::move(hero));
    }

    // No more running, all heroes escaped
    else {
        for (auto& hero : m_heroes)
            heroGetsOut(hero.get());
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
    m_removeHeroes.emplace_back(hero);
}

void HeroesManager::heroLeftRoom(Hero* hero, const sf::Vector2u& coords)
{
    Event event;
    event.type = EventType::HERO_LEFT_ROOM;
    event.action.room = {coords.x, coords.y};
    event.action.hero = hero;
    emitter()->emit(event);
}

void HeroesManager::heroEnteredRoom(Hero* hero, const sf::Vector2u& coords)
{
    Event event;
    event.type = EventType::HERO_ENTERED_ROOM;
    event.action.room = {coords.x, coords.y};
    event.action.hero = hero;
    emitter()->emit(event);
}

void HeroesManager::heroStealsTreasure(Hero* hero, const sf::Vector2u& coords, const uint stolenDosh)
{
    m_data->stealTreasure(coords, *hero, stolenDosh);
}
