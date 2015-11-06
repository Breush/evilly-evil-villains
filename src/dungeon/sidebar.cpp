#include "dungeon/sidebar.hpp"

#include "core/application.hpp"
#include "core/gettext.hpp"
#include "dungeon/event.hpp"
#include "dungeon/monstercage.hpp"
#include "dungeon/facilities.hpp"
#include "dungeon/traps.hpp"
#include "dungeon/tools.hpp"
#include "dungeon/data.hpp"
#include "scene/scene.hpp"
#include "tools/platform-fixes.hpp" // make_unique

using namespace dungeon;

Sidebar::Sidebar(scene::Scene& inScene, Inter& inter, Data& data)
    : m_inter(inter)
    , m_data(data)
    , m_scene(inScene)
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
    m_tabContents[TabsID::MONSTERS].stacker.setPadding(0.f);
    for (auto& tabContent : m_tabContents)
        tabContent.scrollArea.setContent(tabContent.stacker);

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
    m_background.setTexture("dungeon/sidebar/background");

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
    // TODO Correctly adapt the background to the size
    // So that there is no streching
    m_background.setSize({size().x - 2.f, size().y});

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

void Sidebar::refreshTabContents()
{
    // Monsters
    auto& monstersStacker = m_tabContents[TabsID::MONSTERS].stacker;
    auto& monstersCages = m_tabContents[TabsID::MONSTERS].monsterCage;
    monstersStacker.unstackAll();
    monstersCages.clear();

    const auto& monstersList = m_data.monstersDB().get();
    auto monstersCount = monstersList.size();
    monstersCages.reserve(monstersCount);

    // TODO Where that 90 coming from?
    const float monsterCageHeight = 90.f;
    for (const auto& monsterPair : monstersList) {
        const auto& monsterID = monsterPair.first;
        monstersCages.emplace_back(std::make_unique<MonsterCage>(monsterID, m_inter, m_data));
        auto& monsterCage = *monstersCages.back();
        monsterCage.setSize({size().x - 2.f * m_borderThick, monsterCageHeight});
        monstersStacker.stackBack(monsterCage, nui::Align::CENTER);
    }

    // Traps
    auto& trapsStacker = m_tabContents[TabsID::TRAPS].stacker;
    // TODO Have something more expressive than a button (follow monsterCage guidelines)
    auto& trapsButtons = m_tabContents[TabsID::TRAPS].buttons;
    trapsStacker.unstackAll();
    trapsButtons.clear();

    const auto& trapsList = m_data.trapsDB().get();
    auto trapsCount = trapsList.size();
    trapsButtons.reserve(trapsCount);

    for (const auto& trapPair : trapsList) {
        const auto& trapID = trapPair.first;
        // FIXME Cheating here... (should follow monster guidelines)
        trapsButtons.emplace_back(std::make_unique<dungeon::TrapGrabButton>(trapPair.second.name, trapID));
        auto& trapButton = *trapsButtons.back();
        trapsStacker.stackBack(trapButton, nui::Align::CENTER);
    }

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

    // Tools
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
