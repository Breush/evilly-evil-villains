#include "dungeon/sidebar.hpp"

#include "core/application.hpp"
#include "core/gettext.hpp"
#include "dungeon/eventtype.hpp"
#include "dungeon/facilities.hpp"
#include "dungeon/traps.hpp"
#include "dungeon/tools.hpp"
#include "scene/scene.hpp"
#include "tools/platform-fixes.hpp" // make_unique

using namespace dungeon;

Sidebar::Sidebar(scene::Scene& inScene)
    : m_scene(inScene)
{
    // Global stacker
    attachChild(m_globalStacker);
    m_globalStacker.setRelativePosition({0.5f, 0.f});
    m_globalStacker.setRelativeOrigin({0.5f, 0.f});
    m_globalStacker.stackBack(m_summary, nui::Align::CENTER);
    m_globalStacker.stackBack(m_tabHolder, nui::Align::CENTER);
    m_globalStacker.stackBack(m_minimap, nui::Align::CENTER);
    m_globalStacker.stackBack(m_lock, nui::Align::CENTER);

    // Minimap
    m_minimap.setCallbackAction([this] (const sf::Vector2f& position) { m_scene.setViewCenter(position); });

    // Tabs content
    for (auto& tabContent : m_tabContents)
        tabContent.scrollArea.setContent(tabContent.stacker);
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
    m_lock.init();

    // Tabs
    m_tabHolder.stackBack(_("Monsters"),    "dungeon/sidebar/tab/monsters",    m_tabContents[TabsID::MONSTERS].scrollArea);
    m_tabHolder.stackBack(_("Traps"),       "dungeon/sidebar/tab/traps",       m_tabContents[TabsID::TRAPS].scrollArea);
    m_tabHolder.stackBack(_("Facilities"),  "dungeon/sidebar/tab/facilities",  m_tabContents[TabsID::FACILITIES].scrollArea);
    m_tabHolder.stackBack(_("Tools"),       "dungeon/sidebar/tab/tools",       m_tabContents[TabsID::TOOLS].scrollArea);
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
    refreshScrollAreasSize();
    refreshTabContents();
}

void Sidebar::onChildSizeChanges(scene::Entity& child)
{
    if (&child != &m_tabHolder)
        refreshScrollAreasSize();
}

//------------------------//
//----- Dungeon data -----//

void Sidebar::useData(Data& data)
{
    m_summary.useData(data);
    m_lock.useData(data);
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
    height -= m_lock.size().y;
    height -= m_tabHolder.headerSize().y;
    height -= 4.f * m_vPadding;

    // Affect new size to all scroll areas
    for (auto& tabContent : m_tabContents)
        tabContent.scrollArea.setSize({size().x, height});
}

void Sidebar::refreshTabContents()
{
    // TODO Use data info: what is accessible, at what price?

    // Monsters
    auto& monstersStacker = m_tabContents[TabsID::MONSTERS].stacker;
    auto& monstersCages = m_tabContents[TabsID::MONSTERS].monsterCage;
    monstersStacker.unstackAll();
    monstersCages.clear();

    monstersCages.emplace_back(std::make_unique<MonsterCage>(L"creepim"));

    for (auto& monsterCage : monstersCages) {
        monsterCage->setSize({size().x - 2.f * m_borderThick, 80.f});
        monstersStacker.stackBack(*monsterCage, nui::Align::CENTER);
    }

    // Traps
    auto& trapsStacker = m_tabContents[TabsID::TRAPS].stacker;
    auto& trapsButtons = m_tabContents[TabsID::TRAPS].buttons;
    trapsStacker.unstackAll();
    trapsButtons.clear();

    trapsButtons.emplace_back(std::make_unique<dungeon::TrapGrabButton>(_("Pick-pock"), "dungeon/traps/pickpock/icon", L"pickpock"));

    for (auto& trapsButton : trapsButtons)
        trapsStacker.stackBack(*trapsButton, nui::Align::CENTER);

    // Facilities
    auto& facilitiesStacker = m_tabContents[TabsID::FACILITIES].stacker;
    auto& facilitiesButtons = m_tabContents[TabsID::FACILITIES].buttons;
    facilitiesStacker.unstackAll();
    facilitiesButtons.clear();

    facilitiesButtons.emplace_back(std::make_unique<FacilityGrabButton>(_("Treasure"), "dungeon/facilities/treasure/icon", L"treasure"));
    facilitiesButtons.emplace_back(std::make_unique<FacilityGrabButton>(_("Entrance"), "dungeon/facilities/entrance/icon", L"entrance"));
    facilitiesButtons.emplace_back(std::make_unique<FacilityGrabButton>(_("Ladder"),   "dungeon/facilities/ladder/icon",   L"ladder"));

    for (auto& facilitiesButton : facilitiesButtons)
        facilitiesStacker.stackBack(*facilitiesButton, nui::Align::CENTER);

    // Toolss
    auto& toolsStacker = m_tabContents[TabsID::TOOLS].stacker;
    auto& toolsButtons = m_tabContents[TabsID::TOOLS].buttons;
    toolsStacker.unstackAll();
    toolsButtons.clear();

    toolsButtons.emplace_back(std::make_unique<ToolGrabButton>(_("Rooms builder"), dungeon::ToolID::ROOMS_BUILDER));
    toolsButtons.emplace_back(std::make_unique<ToolGrabButton>(_("Rooms destroyer"), dungeon::ToolID::ROOMS_DESTROYER));
    toolsButtons.emplace_back(std::make_unique<ToolGrabButton>(_("Traps remover"), dungeon::ToolID::TRAPS_REMOVER));
    toolsButtons.emplace_back(std::make_unique<ToolGrabButton>(_("Facilities remover"), dungeon::ToolID::FACILITIES_REMOVER));

    for (auto& toolsButton : toolsButtons)
        toolsStacker.stackBack(*toolsButton, nui::Align::CENTER);
}
