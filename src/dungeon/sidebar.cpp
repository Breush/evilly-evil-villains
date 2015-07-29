#include "dungeon/sidebar.hpp"

#include "core/application.hpp"
#include "core/gettext.hpp"
#include "dungeon/eventtype.hpp"
#include "dungeon/facilities.hpp"
#include "dungeon/traps.hpp"
#include "resources/identifiers.hpp"
#include "tools/platform-fixes.hpp" // make_unique

using namespace dungeon;

Sidebar::Sidebar()
{
    // Global stacker
    attachChild(m_globalStacker);
    m_globalStacker.setRelativePosition({0.5f, 0.f});
    m_globalStacker.setRelativeOrigin({0.5f, 0.f});
    m_globalStacker.stackBack(m_summary, nui::Align::CENTER);
    m_globalStacker.stackBack(m_tabHolder, nui::Align::CENTER);
    m_globalStacker.stackBack(m_minimap, nui::Align::CENTER);

    // Background
    // TODO Have a better image...
    m_background.setTexture(&Application::context().textures.get(TextureID::DUNGEON_SIDEBAR_BACKGROUND));
    addPart(&m_background);

    // Tabs + tab content
    m_tabHolder.stackBack(_("Monsters"),    TextureID::DUNGEON_SIDEBAR_TAB_MONSTERS,    m_tabContents[TabsID::MONSTERS].scrollArea);
    m_tabHolder.stackBack(_("Traps"),       TextureID::DUNGEON_SIDEBAR_TAB_TRAPS,       m_tabContents[TabsID::TRAPS].scrollArea);
    m_tabHolder.stackBack(_("Facilities"),  TextureID::DUNGEON_SIDEBAR_TAB_FACILITIES,  m_tabContents[TabsID::FACILITIES].scrollArea);

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

//-------------------//
//----- Routine -----//

void Sidebar::onSizeChanges()
{
    // Background
    m_background.setSize(size());

    // All scroll areas
    for (auto& tabContent : m_tabContents)
        tabContent.scrollArea.setSize({size().x, 0.25f * size().y});
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

void Sidebar::refreshTabContents()
{
    // TODO Use data info: what is accessible, at what price?

    // Monsters
    auto& monstersStacker = m_tabContents[TabsID::MONSTERS].stacker;
    auto& monstersButtons = m_tabContents[TabsID::MONSTERS].buttons;
    monstersStacker.unstackAll();
    monstersButtons.clear();

    for (auto& monstersButton : monstersButtons)
        monstersStacker.stackBack(*monstersButton, nui::Align::CENTER);

    // Traps
    auto& trapsStacker = m_tabContents[TabsID::TRAPS].stacker;
    auto& trapsButtons = m_tabContents[TabsID::TRAPS].buttons;
    trapsStacker.unstackAll();
    trapsButtons.clear();

    trapsButtons.emplace_back(std::make_unique<dungeon::TrapGrabButton>(_("Pick-pock"), TextureID::DUNGEON_TRAPS_PICKPOCK_ICON, L"pickpock"));

    for (auto& trapsButton : trapsButtons)
        trapsStacker.stackBack(*trapsButton, nui::Align::CENTER);

    // Facilities
    auto& facilitiesStacker = m_tabContents[TabsID::FACILITIES].stacker;
    auto& facilitiesButtons = m_tabContents[TabsID::FACILITIES].buttons;
    facilitiesStacker.unstackAll();
    facilitiesButtons.clear();

    facilitiesButtons.emplace_back(std::make_unique<dungeon::FacilityGrabButton>(_("Treasure"), TextureID::DUNGEON_FACILITIES_TREASURE_ICON, L"treasure"));
    facilitiesButtons.emplace_back(std::make_unique<dungeon::FacilityGrabButton>(_("Entrance"), TextureID::DUNGEON_FACILITIES_ENTRANCE_ICON, L"entrance"));
    facilitiesButtons.emplace_back(std::make_unique<dungeon::FacilityGrabButton>(_("Ladder"),   TextureID::DUNGEON_FACILITIES_LADDER_ICON,   L"ladder"));
    // FIXME that duplicate is for testing
    facilitiesButtons.emplace_back(std::make_unique<dungeon::FacilityGrabButton>(_("Ladder"),   TextureID::DUNGEON_FACILITIES_LADDER_ICON,   L"ladder"));
    facilitiesButtons.emplace_back(std::make_unique<dungeon::FacilityGrabButton>(_("Ladder"),   TextureID::DUNGEON_FACILITIES_LADDER_ICON,   L"ladder"));
    facilitiesButtons.emplace_back(std::make_unique<dungeon::FacilityGrabButton>(_("Ladder"),   TextureID::DUNGEON_FACILITIES_LADDER_ICON,   L"ladder"));
    facilitiesButtons.emplace_back(std::make_unique<dungeon::FacilityGrabButton>(_("Ladder"),   TextureID::DUNGEON_FACILITIES_LADDER_ICON,   L"ladder"));
    facilitiesButtons.emplace_back(std::make_unique<dungeon::FacilityGrabButton>(_("Ladder"),   TextureID::DUNGEON_FACILITIES_LADDER_ICON,   L"ladder"));
    facilitiesButtons.emplace_back(std::make_unique<dungeon::FacilityGrabButton>(_("Ladder"),   TextureID::DUNGEON_FACILITIES_LADDER_ICON,   L"ladder"));
    facilitiesButtons.emplace_back(std::make_unique<dungeon::FacilityGrabButton>(_("Ladder"),   TextureID::DUNGEON_FACILITIES_LADDER_ICON,   L"ladder"));
    facilitiesButtons.emplace_back(std::make_unique<dungeon::FacilityGrabButton>(_("Ladder"),   TextureID::DUNGEON_FACILITIES_LADDER_ICON,   L"ladder"));

    for (auto& facilitiesButton : facilitiesButtons)
        facilitiesStacker.stackBack(*facilitiesButton, nui::Align::CENTER);
}
