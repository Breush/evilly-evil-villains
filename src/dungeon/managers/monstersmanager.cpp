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

    // Update monsters
    bool monstersCountChanged = false;
    for (auto it = std::begin(m_monstersInfo); it != std::end(m_monstersInfo); ) {
        auto& monsterInfo = *it;

        // Monster is running
        if (monsterInfo.status == MonsterStatus::RUNNING) {
            // Monster is being damaged
            if (monsterInfo.damageFeedback) {
                monsterInfo.damageFeedbackTime -= dt.asSeconds();
                if (monsterInfo.damageFeedbackTime <= 0.f) {
                    monsterInfo.monster->setDamaged(false);
                    monsterInfo.damageFeedback = false;
                }
            }
        }

        // Spawn monster
        else if (monsterInfo.status == MonsterStatus::TO_SPAWN) {
            monsterInfo.status = MonsterStatus::RUNNING;

            // Create the monster object and add it as an Inter child
            auto& monster = monsterInfo.monster;
            monster = std::make_unique<Monster>(*m_inter, *m_graph);
            monster->bindElementData(monsterInfo.data);
            m_inter->attachChild(*monster);
        }

        // Remove the monster
        else if (monsterInfo.status == MonsterStatus::TO_BE_REMOVED) {
            if (monsterInfo.locked) goto _continue;
            it = m_monstersInfo.erase(it);
            monstersCountChanged = true;
            continue;
        }

        _continue:
        ++it;
    }

    // Monsters count changed, update all reference to their data
    if (monstersCountChanged)
        refreshMonstersData();
}

//------------------//
//----- Events -----//

void MonstersManager::receive(const context::Event& event)
{
    const auto& devent = *reinterpret_cast<const dungeon::Event*>(&event);
    RoomCoords coords = {devent.room.x, devent.room.y};

    if (devent.type == "room_destroyed") {
        removeRoomMonsters(coords);
    }
    else if (devent.type == "dungeon_graph_changed") {
        refreshMonstersFromGraph();
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
        monsterInfo.hp = monsterNode.attribute(L"hp").as_float();

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
        monsterNode.append_attribute(L"hp") = monsterInfo.hp;

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

//----------------------------//
//----- Monsters control -----//

void MonstersManager::damage(const Monster* monster, float amount)
{
    for (auto& monsterInfo : m_monstersInfo) {
        if (monsterInfo.monster.get() != monster) continue;

        monsterInfo.hp -= amount;
        if (monsterInfo.hp <= 0.f)
            monsterInfo.status = MonsterStatus::TO_BE_REMOVED;

        // Player feedback
        monsterInfo.monster->setDamaged(true);
        monsterInfo.damageFeedbackTime = 0.05f;
        monsterInfo.damageFeedback = true;

        return;
    }
}

void MonstersManager::removeRoomMonsters(const RoomCoords& coords)
{
    for (auto& monsterInfo : m_monstersInfo) {
        RoomCoords monsterCoords;
        monsterCoords.x = static_cast<uint>(monsterInfo.data[L"rx"].as_float());
        monsterCoords.y = static_cast<uint>(monsterInfo.data[L"ry"].as_float());

        if (monsterCoords == coords)
            monsterInfo.status = MonsterStatus::TO_BE_REMOVED;
    }
}

void MonstersManager::addRoomMonster(const RoomCoords& coords, const std::wstring& monsterID)
{
    MonsterInfo monsterInfo;
    const auto& monsterData = m_data->monstersDB().get(monsterID);

    monsterInfo.status = MonsterStatus::TO_SPAWN;
    monsterInfo.data.create(monsterID);
    monsterInfo.data[L"rx"].init_float(coords.x + 0.5f);
    monsterInfo.data[L"ry"].init_float(coords.y + 0.5f);
    monsterInfo.hp = monsterData.startingHP;

    m_monstersInfo.emplace_back(std::move(monsterInfo));

    refreshMonstersData();
}

void MonstersManager::listRoomMonsters(const RoomCoords& coords, std::vector<Monster*>& monstersList) const
{
    monstersList.clear();

    for (auto& monsterInfo : m_monstersInfo) {
        if (monsterInfo.status != MonsterStatus::RUNNING) continue;
        if (static_cast<uint>(monsterInfo.data.at(L"rx").as_float()) != coords.x) continue;
        if (static_cast<uint>(monsterInfo.data.at(L"ry").as_float()) != coords.y) continue;

        monstersList.emplace_back(monsterInfo.monster.get());
    }
}

void MonstersManager::setLocked(const Monster* monster, bool locked)
{
    for (auto& monsterInfo : m_monstersInfo) {
        if (monsterInfo.monster.get() != monster) continue;
        if (monsterInfo.status != MonsterStatus::RUNNING) continue;
        monsterInfo.monster->setMoving(!locked);
        monsterInfo.locked = locked;
        return;
    }
}

//---------------//
//----- ICU -----//

void MonstersManager::refreshMonstersData()
{
    for (auto& monsterInfo : m_monstersInfo)
        if (monsterInfo.status == MonsterStatus::RUNNING)
            monsterInfo.monster->bindElementData(monsterInfo.data);
}

void MonstersManager::refreshMonstersFromGraph()
{
    for (auto& monsterInfo : m_monstersInfo)
        if (monsterInfo.status == MonsterStatus::RUNNING)
            monsterInfo.monster->updateFromGraph();
}
