#include "dungeon/managers/dynamicsmanager.hpp"

#include "dungeon/inter.hpp"

using namespace dungeon;

DynamicsManager::DynamicsManager()
{
}

//-------------------//
//----- Routine -----//

void DynamicsManager::update(const sf::Time& dt)
{
    // Update dynamics
    bool dynamicsCountChanged = false;
    for (auto it = std::begin(m_dynamicsInfo); it != std::end(m_dynamicsInfo); ) {
        auto& dynamicInfo = *it;

        // Remove the dynamic
        if (dynamicInfo.status == DynamicStatus::TO_BE_REMOVED) {
            it = m_dynamicsInfo.erase(it);
            dynamicsCountChanged = true;
            continue;
        }

        ++it;
    }

    // Dynamics count changed, update all reference to their data
    // TODO See HeroesManager comment
    if (dynamicsCountChanged)
        refreshDynamicsData();
}

//------------------//
//----- Events -----//

void DynamicsManager::receive(const context::Event& event)
{
    /*
    const auto& devent = *reinterpret_cast<const dungeon::Event*>(&event);
    sf::Vector2u coords = {devent.room.x, devent.room.y};
    */
}

//----------------------------//
//----- File interaction -----//

void DynamicsManager::load(const pugi::xml_node& node)
{
    m_dynamicsInfo.clear();

    for (const auto& dynamicNode : node.children(L"dynamic")) {
        m_dynamicsInfo.emplace_back();
        auto& dynamicInfo = m_dynamicsInfo.back();
        dynamicInfo.data.loadXML(dynamicNode);
    }

    refreshDynamicsData();
}

void DynamicsManager::save(pugi::xml_node node)
{
    for (const auto& dynamicInfo : m_dynamicsInfo) {
        auto dynamicNode = node.append_child(L"dynamic");
        dynamicInfo.data.saveXML(dynamicNode);
    }
}

//-------------------------------//
//----- Dungeon interaction -----//

void DynamicsManager::useInter(Inter& inter)
{
    m_inter = &inter;
    m_data = &m_inter->data();
    setEmitter(m_data);
}

//----------------------------//
//----- Dynamics control -----//

uint32 DynamicsManager::create(const sf::Vector2f& rpos, const std::wstring& id)
{
    DynamicInfo dynamicInfo;
    dynamicInfo.data.create(id);
    dynamicInfo.data[L"rx"].init_float(rpos.x);
    dynamicInfo.data[L"ry"].init_float(rpos.y);
    dynamicInfo.status = DynamicStatus::RUNNING;
    dynamicInfo.dynamic = std::make_unique<Dynamic>(*m_inter);
    m_dynamicsInfo.emplace_back(std::move(dynamicInfo));

    refreshDynamicsData();

    auto& dynamic = *m_dynamicsInfo.back().dynamic;
    m_inter->attachChild(dynamic);
    return dynamic.UID();
}

void DynamicsManager::remove(const Dynamic* pDynamic)
{
    for (auto& dynamicInfo : m_dynamicsInfo)
        if (dynamicInfo.dynamic.get() == pDynamic)
            dynamicInfo.status = DynamicStatus::TO_BE_REMOVED;
}

//---------------//
//----- ICU -----//

void DynamicsManager::refreshDynamicsData()
{
    for (auto& dynamicInfo : m_dynamicsInfo)
        dynamicInfo.dynamic->bindElementData(dynamicInfo.data);
}
