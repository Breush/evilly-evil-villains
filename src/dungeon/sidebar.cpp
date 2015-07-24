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
    // Background
    // TODO Big cheating here on rotation, better have a vertical texture too
    m_background.setTexture(&Application::context().textures.get(TextureID::DUNGEON_PANEL_BACKGROUND));
    m_background.setRotation(90);
    addPart(&m_background);

    // Tabs
    attachChild(m_tabsStacker);
    m_tabsStacker.setRelativeOrigin({0.5f, 0.f});
}

Sidebar::~Sidebar()
{
    m_tabsStacker.unstackAll();
    m_tabs.clear();
}

//-------------------//
//----- Routine -----//

void Sidebar::onSizeChanges()
{
    // Background
    m_background.setPosition({size().x, 0.f});
    m_background.setSize({size().y, size().x});

    m_tabsStacker.setLocalPosition({0.5f * size().x, 50.f});
}

//------------------------//
//----- Mouse events -----//

void Sidebar::handleMouseButtonPressed(const sf::Mouse::Button, const sf::Vector2f& mousePos, const sf::Vector2f&)
{
}

void Sidebar::handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    resetPartsShader();
}

void Sidebar::handleMouseLeft()
{
    resetPartsShader();
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
    m_tabsStacker.unstackAll();
    m_tabs.clear();

    switch (mode) {
    case Mode::MONSTERS:
        break;

    case Mode::TRAPS:
        m_tabs.emplace_back(std::make_unique<dungeon::TrapGrabButton>(_("Pick-pock"), TextureID::DUNGEON_TRAPS_PICKPOCK_ICON, L"pickpock"));
        break;

    case Mode::FACILITIES:
        m_tabs.emplace_back(std::make_unique<dungeon::FacilityGrabButton>(_("Treasure"), TextureID::DUNGEON_FACILITIES_TREASURE_ICON, L"treasure"));
        m_tabs.emplace_back(std::make_unique<dungeon::FacilityGrabButton>(_("Entrance"), TextureID::DUNGEON_FACILITIES_ENTRANCE_ICON, L"entrance"));
        m_tabs.emplace_back(std::make_unique<dungeon::FacilityGrabButton>(_("Ladder"),   TextureID::DUNGEON_FACILITIES_LADDER_ICON,   L"ladder"));
        break;
    }

    // Add tabs to stacker
    for (auto& tab : m_tabs)
        m_tabsStacker.stackBack(*tab.get(), nui::Align::CENTER);
}
