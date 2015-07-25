#include "dungeon/panel.hpp"

#include "core/application.hpp"
#include "core/gettext.hpp"
#include "resources/identifiers.hpp"
#include "dungeon/sidebar.hpp"
#include "sfe/lerpable.hpp"
#include "tools/math.hpp"

using namespace dungeon;

Panel::Panel(Sidebar& sidebar)
    : baseClass(true) // Lerpable
    , m_width(0.f)
    , m_sidebar(sidebar)
{
    // Tabs stacker
    attachChild(m_tabsStacker);
    m_tabsStacker.centerOrigin();
    m_tabsStacker.setRelativePosition({0.5f, 0.5f});
    for (auto& tab : m_tabs)
        m_tabsStacker.stackBack(tab, nui::Align::CENTER);

    // Tabs
    m_tabs[0].setImage(TextureID::DUNGEON_PANEL_MONSTERS);
    m_tabs[1].setImage(TextureID::DUNGEON_PANEL_TRAPS);
    m_tabs[2].setImage(TextureID::DUNGEON_PANEL_FACILITIES);

    m_tabs[0].setCallback([&]() { m_sidebar.setMode(Sidebar::Mode::MONSTERS); });
    m_tabs[1].setCallback([&]() { m_sidebar.setMode(Sidebar::Mode::TRAPS); });
    m_tabs[2].setCallback([&]() { m_sidebar.setMode(Sidebar::Mode::FACILITIES); });
}

//-------------------//
//----- Routine -----//

void Panel::onChildSizeChanges(scene::Entity& child)
{
    m_height = child.size().y;
    refreshSize();
}

//--------------------------//
//----- Dungeon events -----//

void Panel::receive(const Event& event)
{
    returnif (event.type != EventType::MODE_CHANGED);
    setVisible(event.mode != Mode::INVASION);
}

//-----------------------------------//
//----- Internal change updates -----//

void Panel::refreshSize()
{
    setSize({m_width, m_height});
}
