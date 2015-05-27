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
    , m_reduced(false)
    , m_sidebar(sidebar)
{
    // Background
    m_background.setTexture(&Application::context().textures.get(TextureID::DUNGEON_PANEL_BACKGROUND));
    addPart(&m_background);

    // Reduced button
    m_switchReducedButton.setTexture(&Application::context().textures.get(TextureID::DUNGEON_PANEL_SWITCH));
    m_switchReducedButton.setSize({20.f, 20.f});
    addPart(&m_switchReducedButton);

    // Tabs stacker
    attachChild(m_tabsStacker);
    m_tabsStacker.centerOrigin();
    for (auto& tab : m_tabs)
        m_tabsStacker.stackBack(tab, nui::Align::CENTER);

    // Tabs
    m_tabs[0].setVisual(_("Monsters"),   TextureID::DUNGEON_PANEL_MONSTERS);
    m_tabs[1].setVisual(_("Traps"),      TextureID::DUNGEON_PANEL_TRAPS);
    m_tabs[2].setVisual(_("Facilities"), TextureID::DUNGEON_PANEL_FACILITIES);

    m_tabs[0].setCallback([&]() { m_sidebar.setMode(Sidebar::Mode::MONSTERS); });
    m_tabs[1].setCallback([&]() { m_sidebar.setMode(Sidebar::Mode::TRAPS); });
    m_tabs[2].setCallback([&]() { m_sidebar.setMode(Sidebar::Mode::FACILITIES); });
}

//-------------------//
//----- Routine -----//

void Panel::onSizeChanges()
{
    // Background
    m_background.setSize(size());

    // Reduced button
    m_switchReducedButton.setPosition({size().x - 40.f, 10.f});

    // Tabs stacker
    m_tabsStacker.setLocalPosition(size() / 2.f);
}

//------------------------//
//----- Mouse events -----//

void Panel::handleMouseButtonPressed(const sf::Mouse::Button, const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    if (m_switchReducedButton.getGlobalBounds().contains(mousePos))
        switchReduced();
}

void Panel::handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    resetPartsShader();

    if (m_switchReducedButton.getGlobalBounds().contains(mousePos))
        setPartShader(&m_switchReducedButton, ShaderID::NUI_HOVER);
}

void Panel::handleMouseLeft()
{
    resetPartsShader();
}

//--------------------------//
//----- Dungeon events -----//

void Panel::receive(const Event& event)
{
    returnif (event.type != EventType::MODE_CHANGED);
    setVisible(event.mode != Mode::INVASION);
}

//------------------------//
//----- Reduced mode -----//

void Panel::setReduced(bool reduced)
{
    m_reduced = reduced;

    if (m_reduced) lerpable()->setTargetPositionOffset({0.f, size().y - 30.f});
    else lerpable()->setTargetPositionOffset({0.f, 0.f});
}

void Panel::switchReduced()
{
    setReduced(!reduced());
}
