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
    : m_tabs(*this)
{
    // Global stacker
    attachChild(m_globalStacker);
    m_globalStacker.setRelativePosition({0.5f, 0.f});
    m_globalStacker.setRelativeOrigin({0.5f, 0.f});
    m_globalStacker.stackBack(m_tabs, nui::Align::CENTER);
    m_globalStacker.stackBack(m_tabContentStacker, nui::Align::CENTER);

    // Background
    // TODO Have a better image...
    m_background.setTexture(&Application::context().textures.get(TextureID::DUNGEON_SIDEBAR_BACKGROUND));
    addPart(&m_background);

    // Tabs + tab content
    m_tabContentStacker.setRelativeOrigin({0.5f, 0.f});
}

Sidebar::~Sidebar()
{
    m_tabContentStacker.unstackAll();
    m_tabContent.clear();
}

//-------------------//
//----- Routine -----//

void Sidebar::onSizeChanges()
{
    // Background
    m_background.setSize(size());

    // Tabs + tab content
    m_tabs.setWidth(size().x);
}

//--------------------------//
//----- Dungeon events -----//

void Sidebar::receive(const Event& event)
{
    returnif (event.type != EventType::MODE_CHANGED);
    setVisible(event.mode != dungeon::Mode::INVASION);
}

//----------------//
//----- Mode -----//

void Sidebar::setMode(Mode mode)
{
    m_tabContentStacker.unstackAll();
    m_tabContent.clear();

    switch (mode) {
    case Mode::MONSTERS:
        break;

    case Mode::TRAPS:
        m_tabContent.emplace_back(std::make_unique<dungeon::TrapGrabButton>(_("Pick-pock"), TextureID::DUNGEON_TRAPS_PICKPOCK_ICON, L"pickpock"));
        break;

    case Mode::FACILITIES:
        m_tabContent.emplace_back(std::make_unique<dungeon::FacilityGrabButton>(_("Treasure"), TextureID::DUNGEON_FACILITIES_TREASURE_ICON, L"treasure"));
        m_tabContent.emplace_back(std::make_unique<dungeon::FacilityGrabButton>(_("Entrance"), TextureID::DUNGEON_FACILITIES_ENTRANCE_ICON, L"entrance"));
        m_tabContent.emplace_back(std::make_unique<dungeon::FacilityGrabButton>(_("Ladder"),   TextureID::DUNGEON_FACILITIES_LADDER_ICON,   L"ladder"));
        break;
    }

    // Add tab content to stacker
    for (auto& tabContent : m_tabContent)
        m_tabContentStacker.stackBack(*tabContent, nui::Align::CENTER);
}
