#include "dungeon/managers/monstersmanager.hpp"

#include "dungeon/inter.hpp"
#include "context/villains.hpp"
#include "tools/platform-fixes.hpp"
#include "tools/random.hpp"

using namespace dungeon;

MonstersManager::MonstersManager()
{
}

//-------------------//
//----- Routine -----//

void MonstersManager::update(const sf::Time& dt)
{
    returnif (m_graph == nullptr);

    // Update heroes
    bool monstersCountChanged = false;
    for (auto it = std::begin(m_monstersInfo); it != std::end(m_monstersInfo); ) {
        auto& monsterInfo = *it;

        // Spawn monster
        if (monsterInfo.status == MonsterStatus::TO_SPAWN) {
            monsterInfo.status = MonsterStatus::RUNNING;

            // Create the monster object and add it as an Inter child
            auto& monster = monsterInfo.monster;
            monster = std::make_unique<Monster>(*m_inter, *m_graph);
            monster->bindElementData(monsterInfo.data);
            m_inter->attachChild(*monster);

            std::cerr << "Spawn" << std::endl;
        }

        // Remove the monster
        else if (monsterInfo.status == MonsterStatus::TO_BE_REMOVED) {
            it = m_monstersInfo.erase(it);
            monstersCountChanged = true;
            continue;
        }

        ++it;
    }

    // Heroes count changed, update all reference to their data
    // TODO See HeroesManager
    if (monstersCountChanged)
        refreshMonstersData();
}

//------------------//
//----- Events -----//

void MonstersManager::receive(const context::Event& event)
{
    const auto& devent = *reinterpret_cast<const dungeon::Event*>(&event);
    sf::Vector2u coords = {devent.room.x, devent.room.y};

    // TODO See HeroesManager
    if (devent.type == "room_destroyed") {
        removeRoomMonsters(coords);
    }
    else if (devent.type == "dungeon_graph_changed") {
        refreshMonstersData();
    }
}

//----------------------------//
//----- File interaction -----//

void MonstersManager::load(const pugi::xml_node& node)
{
    m_monstersInfo.clear();

    for (const auto& monsterNode : node.children(L"monster")) {
        m_monstersInfo.emplace_back();
        auto& monsterInfo = m_monstersInfo.back();
        monsterInfo.data.loadXML(monsterNode);

        std::wstring status = monsterNode.attribute(L"status").as_string();
        if (status == L"spawning") {
            monsterInfo.status = MonsterStatus::TO_SPAWN;
        }
        else if (status == L"running") {
            monsterInfo.status = MonsterStatus::TO_SPAWN;
        }
    }

    refreshMonstersData();
}

void MonstersManager::save(pugi::xml_node node)
{
    for (const auto& monsterInfo : m_monstersInfo) {
        if (monsterInfo.status == MonsterStatus::TO_BE_REMOVED) continue;

        auto monsterNode = node.append_child(L"monster");
        monsterInfo.data.saveXML(monsterNode);

        if (monsterInfo.status == MonsterStatus::TO_SPAWN) {
            monsterNode.append_attribute(L"status") = L"spawning";
        }
        else if (monsterInfo.status == MonsterStatus::RUNNING) {
            monsterNode.append_attribute(L"status") = L"running";
        }
    }
}

//-------------------------------//
//----- Dungeon interaction -----//

void MonstersManager::useInter(Inter& inter)
{
    m_inter = &inter;
    m_data = &m_inter->data();
    setEmitter(m_data);
}

void MonstersManager::useGraph(Graph& graph)
{
    m_graph = &graph;
}

//-------------------//
//----- Control -----//

void MonstersManager::removeRoomMonsters(const sf::Vector2u& coords)
{
    for (auto& monsterInfo : m_monstersInfo) {
        sf::Vector2u monsterCoords;
        monsterCoords.x = static_cast<uint>(monsterInfo.data[L"rx"].as_float());
        monsterCoords.y = static_cast<uint>(monsterInfo.data[L"ry"].as_float());

        if (monsterCoords == coords)
            monsterInfo.status = MonsterStatus::TO_BE_REMOVED;
    }
}

void MonstersManager::addRoomMonster(const sf::Vector2u& coords, const std::wstring& monsterID)
{
    MonsterInfo monsterInfo;

    monsterInfo.status = MonsterStatus::TO_SPAWN;
    monsterInfo.data.create(monsterID);
    monsterInfo.data[L"rx"].init_float(coords.x + 0.5f);
    monsterInfo.data[L"ry"].init_float(coords.y + 0.5f);

    m_monstersInfo.emplace_back(std::move(monsterInfo));

    refreshMonstersData();
}

//---------------//
//----- ICU -----//

void MonstersManager::refreshMonstersData()
{
    for (auto& monsterInfo : m_monstersInfo)
        if (monsterInfo.status == MonsterStatus::RUNNING)
            monsterInfo.monster->bindElementData(monsterInfo.data);
}
