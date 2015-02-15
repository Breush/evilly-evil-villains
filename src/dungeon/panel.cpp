#include "dungeon/panel.hpp"

#include "core/application.hpp"
#include "sfe/lerpable.hpp"
#include "tools/math.hpp"

using namespace dungeon;

Panel::Panel()
    : baseClass(true)
    , m_reduced(false)
{
    // Background
    m_background.setTexture(&Application::context().textures.get(Textures::DUNGEON_PANEL_BACKGROUND));

    // Reduced button
    m_switchReducedButton.setTexture(&Application::context().textures.get(Textures::DUNGEON_PANEL_SWITCH));
    m_switchReducedButton.setSize({20.f, 20.f});

    // Tabs stacker
    attachChild(m_tabsStacker);

    // TODO Make a std::array to store tabs
    m_tabsStacker.add(&m_monstersTab,   nui::Stacker::Align::CENTER);
    m_tabsStacker.add(&m_trapsTab,      nui::Stacker::Align::CENTER);
    m_tabsStacker.add(&m_facilitiesTab, nui::Stacker::Align::CENTER);
    m_tabsStacker.add(&m_treasuresTab,  nui::Stacker::Align::CENTER);

    // Tabs
    // TODO Get tabImageSize from somewhere
    sf::Vector2f tabImageSize(80.f, 80.f);
    m_monstersTab.setVisual  (_("Monsters"),   Textures::DUNGEON_PANEL_MONSTERS,   tabImageSize);
    m_trapsTab.setVisual     (_("Traps"),      Textures::DUNGEON_PANEL_TRAPS,      tabImageSize);
    m_facilitiesTab.setVisual(_("Facilities"), Textures::DUNGEON_PANEL_FACILITIES, tabImageSize);
    m_treasuresTab.setVisual (_("Treasures"),  Textures::DUNGEON_PANEL_TREASURES,  tabImageSize);

    update();
}

void Panel::update()
{
    clearParts();

    // Background
    m_background.setSize(size());
    addPart(&m_background);

    // Reduced button
    m_switchReducedButton.setPosition({size().x - 40.f, 10.f});
    addPart(&m_switchReducedButton);

    // Tabs stacker
    m_tabsStacker.setAlign(nui::Stacker::CENTER);
    m_tabsStacker.setSize(size());
}

//------------------------//
//----- Mouse events -----//

void Panel::handleMouseButtonPressed(const sf::Mouse::Button&, const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    if (m_switchReducedButton.getGlobalBounds().contains(mousePos))
        switchReduced();
}

void Panel::handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    resetPartsShader();

    if (m_switchReducedButton.getGlobalBounds().contains(mousePos))
        setPartShader(&m_switchReducedButton, Shaders::NUI_HOVER);
}

void Panel::handleMouseLeft()
{
    resetPartsShader();
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
