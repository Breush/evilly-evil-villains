#include "dungeon/managers/heroesmanager.hpp"

#include "dungeon/inter.hpp"
#include "context/villains.hpp"
#include "tools/platform-fixes.hpp"
#include "tools/random.hpp"

using namespace dungeon;

HeroesManager::HeroesManager()
{
}

//-------------------//
//----- Routine -----//

void HeroesManager::update(const sf::Time& dt)
{
    returnif (m_graph == nullptr);

    // Update heroes
    bool heroesCountChanged = false;
    for (auto it = std::begin(m_heroesInfo); it != std::end(m_heroesInfo); ) {
        auto& heroInfo = *it;

        // Spawn hero or update delay before spawning
        if (heroInfo.status == HeroStatus::TO_SPAWN) {
            heroInfo.spawnDelay -= dt.asSeconds();
            if (heroInfo.spawnDelay > 0.f) continue;
            heroInfo.status = HeroStatus::RUNNING;

            // No entrance? Hero is disappointed
            const auto& startingNodes = m_graph->startingNodes();
            if (startingNodes.empty()) {
                heroInfo.status = HeroStatus::TO_BE_REMOVED;
                Application::context().sounds.play("dungeon/heroes/no_entrance");
                m_data->fameWallet().sub(2u);
                continue;
            }

            // Choose an entrance
            auto startingNode = alea::rand(startingNodes);
            auto coords = toNodeData(startingNode)->coords;
            heroInfo.data[L"rx"].init_float(coords.x + 0.5f);
            heroInfo.data[L"ry"].init_float(coords.y + 0.5f);

            // Create the hero object and add it as an Inter child
            auto& hero = heroInfo.hero;
            hero = std::make_unique<Hero>(*this, *m_inter, *m_graph);
            hero->bindElementData(heroInfo.data);
            m_inter->attachChild(*hero);
        }

        // Remove the hero
        else if (heroInfo.status == HeroStatus::TO_BE_REMOVED) {
            if (heroInfo.reward && heroInfo.hero != nullptr)
                heroInfo.hero->onDeath();

            it = m_heroesInfo.erase(it);
            heroesCountChanged = true;
            continue;
        }

        ++it;
    }

    // Heroes count changed, update all reference to their data
    // TODO This could use a lighter version of refreshHeroesData
    // because we are only interested in keeping the reference (pointer) OK,
    // and not to reinitialize LUA/etc.
    // Ultimately, we won't need _reinit() function in lua (just one _register on _init)
    if (heroesCountChanged)
        refreshHeroesData();

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
    sf::Vector2u coords = {devent.room.x, devent.room.y};

    // FIXME This event should slowly kills the hero via asphyxia
    if (devent.type == "room_destroyed") {
        // Remove all heroes in that room
        for (auto& heroInfo : m_heroesInfo) {
            sf::Vector2u heroCoords;
            heroCoords.x = static_cast<uint>(heroInfo.data[L"rx"].as_float());
            heroCoords.y = static_cast<uint>(heroInfo.data[L"ry"].as_float());

            if (heroCoords == coords) {
                heroInfo.status = HeroStatus::TO_BE_REMOVED;
                heroInfo.reward = true;
            }
        }
    }
    else if (devent.type == "dungeon_graph_changed") {
        refreshHeroesData();
    }
}

//----------------------------//
//----- File interaction -----//

void HeroesManager::load(const pugi::xml_node& node)
{
    m_heroesInfo.clear();

    m_nextGroupDelay = node.attribute(L"nextWaveDelay").as_float(10.f);

    for (const auto& heroNode : node.children(L"hero")) {
        m_heroesInfo.emplace_back();
        auto& heroInfo = m_heroesInfo.back();
        heroInfo.data.loadXML(heroNode);
        heroInfo.hp = heroNode.attribute(L"hp").as_float();

        std::wstring status = heroNode.attribute(L"status").as_string();
        if (status == L"spawning") {
            heroInfo.status = HeroStatus::TO_SPAWN;
            heroInfo.spawnDelay = heroNode.attribute(L"spawnDelay").as_float();
        }
        else if (status == L"running") {
            heroInfo.status = HeroStatus::TO_SPAWN;
            heroInfo.spawnDelay = 0.f;
        }
    }

    refreshHeroesData();
}

void HeroesManager::save(pugi::xml_node node)
{
    node.append_attribute(L"nextWaveDelay") = m_nextGroupDelay;

    for (const auto& heroInfo : m_heroesInfo) {
        if (heroInfo.status == HeroStatus::TO_BE_REMOVED) continue;

        auto heroNode = node.append_child(L"hero");
        heroInfo.data.saveXML(heroNode);
        heroNode.append_attribute(L"hp") = heroInfo.hp;

        if (heroInfo.status == HeroStatus::TO_SPAWN) {
            heroNode.append_attribute(L"status") = L"spawning";
            heroNode.append_attribute(L"spawnDelay") = heroInfo.spawnDelay;
        }
        else if (heroInfo.status == HeroStatus::RUNNING) {
            heroNode.append_attribute(L"status") = L"running";
        }
    }
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
    uint newHeroesCount = 1u + rand() % 5u;

    float delay = 0.f;
    for (uint i = 0u; i < newHeroesCount; ++i) {
        m_heroesInfo.emplace_back();
        auto& heroInfo = m_heroesInfo.back();

        // FIXME We're spawning only Gr'oo right now...
        // We should make a more complex group theory
        std::wstring heroID(L"groo");
        const auto& heroData = m_data->heroesDB().get(heroID);

        heroInfo.data.create(heroID);
        heroInfo.spawnDelay = delay;
        heroInfo.hp = heroData.startingHP;

        delay += 3.f + static_cast<float>(rand() % 20u);
    }

    // Refresh all element data, as pending reference might be lost
    refreshHeroesData();

    m_nextGroupDelay = 37.f + static_cast<float>(rand() % 120u);
}

//--------------------------//
//----- Heroes control -----//

void HeroesManager::damage(const Hero* hero, float amount)
{
    for (auto& heroInfo : m_heroesInfo) {
        if (heroInfo.hero.get() != hero) continue;

        heroInfo.hp -= amount;
        if (heroInfo.hp <= 0.f) {
            heroInfo.status = HeroStatus::TO_BE_REMOVED;
            heroInfo.reward = true;
        }

        return;
    }
}

//---------------------------//
//----- Heroes feedback -----//

void HeroesManager::heroGetsOut(Hero* hero)
{
    auto pHeroInfo = std::find_if(m_heroesInfo, [hero] (const HeroInfo& heroInfo) { return heroInfo.hero.get() == hero; });
    returnif (pHeroInfo == std::end(m_heroesInfo));
    auto& heroInfo = *pHeroInfo;

    // If no dosh stolen, hero is unhappy, fame decrease
    // TODO This should be part of Hero Lua
    if (heroInfo.data[L"dosh"].as_uint32() == 0u)
        m_data->fameWallet().sub(4u);
    else
        m_data->fameWallet().add(1u);

    // Remove hero from list
    heroInfo.status = HeroStatus::TO_BE_REMOVED;
    hero->setVisible(false);
}

uint HeroesManager::heroStealsTreasure(Hero*, const sf::Vector2u& coords, const uint stolenDosh)
{
    return m_data->stealRoomTreasure(coords, stolenDosh);
}

//---------------//
//----- ICU -----//

void HeroesManager::refreshHeroesData()
{
    for (auto& heroInfo : m_heroesInfo)
        if (heroInfo.status == HeroStatus::RUNNING)
            heroInfo.hero->bindElementData(heroInfo.data);
}