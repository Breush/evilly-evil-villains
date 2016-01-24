#include "dungeon/sidebar/sidebar.hpp"

#include "context/context.hpp"
#include "core/gettext.hpp"
#include "dungeon/event.hpp"
#include "dungeon/sidebar/monstercage.hpp"
#include "dungeon/sidebar/trapbutton.hpp"
#include "dungeon/sidebar/facilitybutton.hpp"
#include "dungeon/sidebar/toolbutton.hpp"
#include "dungeon/data.hpp"
#include "scene/graph.hpp"
#include "tools/platform-fixes.hpp" // make_unique

using namespace dungeon;

Sidebar::Sidebar(scene::Graph& graph, Inter& inter, Data& data)
    : m_inter(inter)
    , m_data(data)
    , m_scene(graph.scene())
    , m_minimap(graph)
{
    // Background
    attachChild(m_background);
    m_background.setLocalPosition({1.f, 0.f});
    m_background.setOutlineColor(sf::Color::White);
    m_background.setOutlineThickness(1.f);

    // Global stacker
    attachChild(m_globalStacker);
    m_globalStacker.setRelativePosition({0.5f, 0.f});
    m_globalStacker.setRelativeOrigin({0.5f, 0.f});
    m_globalStacker.stackBack(m_summary, nui::Align::CENTER);
    m_globalStacker.stackBack(m_tabHolder, nui::Align::CENTER);
    m_globalStacker.stackBack(m_minimap, nui::Align::CENTER);

    // Tabs
    for (auto& tabContent : m_tabContents)
        tabContent.scrollArea.setContent(tabContent.stacker);

    auto& monstersStacker = m_tabContents[TabsID::MONSTERS].stacker;
    monstersStacker.setPadding(0.f);

    // Minimap
    m_minimap.setCallbackAction([this] (const sf::Vector2f& position) { m_scene.setViewCenter(position); });
}

Sidebar::~Sidebar()
{
    for (auto& tabContent : m_tabContents) {
        tabContent.stacker.unstackAll();
        tabContent.buttons.clear();
    }
}

void Sidebar::init()
{
    m_minimap.init();
    m_summary.init();

    // Background
    m_background.setTexture("core/dungeon/sidebar/background");

    // Tabs
    m_tabHolder.stackBack(_("Monsters"),    "core/dungeon/sidebar/tabs/monsters/icon",    m_tabContents[TabsID::MONSTERS].scrollArea);
    m_tabHolder.stackBack(_("Traps"),       "core/dungeon/sidebar/tabs/traps/icon",       m_tabContents[TabsID::TRAPS].scrollArea);
    m_tabHolder.stackBack(_("Facilities"),  "core/dungeon/sidebar/tabs/facilities/icon",  m_tabContents[TabsID::FACILITIES].scrollArea);
    m_tabHolder.stackBack(_("Tools"),       "core/dungeon/sidebar/tabs/tools/icon",       m_tabContents[TabsID::TOOLS].scrollArea);

    // Monsters
    auto& monstersCages = m_tabContents[TabsID::MONSTERS].monsterCages;
    const auto& monstersList = m_data.monstersGenerics();
    for (const auto& monsterPair : monstersList) {
        const auto& monsterID = monsterPair.first;
        auto monsterCage = std::make_unique<MonsterCage>(monsterID, m_inter, m_data);
        monstersCages.emplace_back(std::move(monsterCage));
    }
}

//-------------------//
//----- Routine -----//

void Sidebar::refreshNUI(const config::NUIGuides& cNUI)
{
    baseClass::refreshNUI(cNUI);

    m_vPadding = cNUI.vPadding;
    m_borderThick = cNUI.borderThick;

    refreshScrollAreasSize();
}

void Sidebar::onSizeChanges()
{
    // Summary
    m_summary.setWidth(size().x);

    // TODO Correctly adapt the background to the size
    // So that there is no streching
    m_background.setSize({size().x - 2.f, size().y});

    refreshScrollAreasSize();
    refreshTabParameters();
}

void Sidebar::onChildSizeChanges(scene::Entity& child)
{
    if (&child != &m_tabHolder)
        refreshScrollAreasSize();
}

//------------------//
//----- Events -----//

void Sidebar::receive(const context::Event& event)
{
    const auto& devent = reinterpret_cast<const dungeon::Event&>(event);

    if (devent.type == "trap_generic_changed")
        refreshTabTrapsContent();
    else if (devent.type == "monster_generic_changed")
        refreshTabMonstersContent();
}

//------------------------//
//----- Dungeon data -----//

// TODO CleanUp Why is there a useData if passed to the constructor?
void Sidebar::useData(Data& data)
{
    setEmitter(&data);
    m_summary.useData(data);
    refreshTabContents();
}

//-----------------------------------//
//----- Internal change updates -----//

void Sidebar::refreshScrollAreasSize()
{
    // Compute new height
    float height = size().y;

    height -= m_minimap.size().y;
    height -= m_summary.size().y;
    height -= m_tabHolder.headerSize().y;
    height -= 3.f * m_vPadding;

    // Affect new size to all scroll areas
    for (auto& tabContent : m_tabContents)
        tabContent.scrollArea.setSize({size().x, height});
}

void Sidebar::refreshTabMonstersContent()
{
    auto& monstersStacker = m_tabContents[TabsID::MONSTERS].stacker;
    auto& monstersCages = m_tabContents[TabsID::MONSTERS].monsterCages;
    monstersStacker.unstackAll();

    // Show only the cages with unlocked monsters
    for (const auto& monsterCage : monstersCages) {
        if (!monsterCage->monsterGeneric().unlocked) continue;
        monstersStacker.stackBack(*monsterCage);
    }

    refreshTabParameters();
}

void Sidebar::refreshTabTrapsContent()
{
    // TODO Have something more expressive than a button (follow monsterCage guidelines)
    // Well, do that for facilities too

    auto& trapsStacker = m_tabContents[TabsID::TRAPS].stacker;
    auto& trapsButtons = m_tabContents[TabsID::TRAPS].buttons;
    trapsStacker.unstackAll();
    trapsButtons.clear();

    const auto& trapsGenerics = m_data.trapsGenerics();
    for (const auto& trapGenericPair : trapsGenerics) {
        if (!trapGenericPair.second.unlocked) continue;
        const auto& trapID = trapGenericPair.first;
        trapsButtons.emplace_back(std::make_unique<TrapGrabButton>(trapGenericPair.second.common->name, trapID));
        auto& trapButton = *trapsButtons.back();
        trapsStacker.stackBack(trapButton);
    }
}

void Sidebar::refreshTabFacilitiesContent()
{
    auto& facilitiesStacker = m_tabContents[TabsID::FACILITIES].stacker;
    auto& facilitiesButtons = m_tabContents[TabsID::FACILITIES].buttons;
    facilitiesStacker.unstackAll();
    facilitiesButtons.clear();

    const auto& facilitiesList = m_data.facilitiesDB().get();
    auto facilitiesCount = m_data.facilitiesDB().listedCount();
    facilitiesButtons.reserve(facilitiesCount);

    for (const auto& facilityPair : facilitiesList) {
        if (!facilityPair.second.listed) continue;
        const auto& facilityID = facilityPair.first;
        facilitiesButtons.emplace_back(std::make_unique<FacilityGrabButton>(facilityPair.second, facilityID, m_inter));
        auto& facilityButton = *facilitiesButtons.back();
        facilitiesStacker.stackBack(facilityButton);
    }
}

void Sidebar::refreshTabToolsContent()
{
    // TODO Merge facilities, tools and mechanics

    auto& toolsStacker = m_tabContents[TabsID::TOOLS].stacker;
    auto& toolsButtons = m_tabContents[TabsID::TOOLS].buttons;
    toolsStacker.unstackAll();
    toolsButtons.clear();

    toolsButtons.emplace_back(std::make_unique<ToolGrabButton>(_("Rooms builder"), dungeon::ToolID::ROOMS_BUILDER));
    toolsButtons.emplace_back(std::make_unique<ToolGrabButton>(_("Rooms destroyer"), dungeon::ToolID::ROOMS_DESTROYER));
    toolsButtons.emplace_back(std::make_unique<ToolGrabButton>(_("Traps remover"), dungeon::ToolID::TRAPS_REMOVER));
    toolsButtons.emplace_back(std::make_unique<ToolGrabButton>(_("Facilities remover"), dungeon::ToolID::FACILITIES_REMOVER));

    for (auto& toolsButton : toolsButtons)
        toolsStacker.stackBack(*toolsButton);
}

void Sidebar::refreshTabContents()
{
    refreshTabMonstersContent();
    refreshTabTrapsContent();
    refreshTabFacilitiesContent();
    refreshTabToolsContent();
}

void Sidebar::refreshTabParameters()
{
    auto& monstersCages = m_tabContents[TabsID::MONSTERS].monsterCages;

    // Monsters
    // TODO Where that 90 coming from?
    const float monsterCageHeight = 90.f;
    for (auto& monsterCage : monstersCages)
        monsterCage->setSize({size().x - 2.f * m_borderThick, monsterCageHeight});
}
