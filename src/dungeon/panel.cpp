#include "dungeon/panel.hpp"

#include "core/application.hpp"
#include "sfe/lerpable.hpp"
#include "tools/math.hpp"

using namespace dungeon;

Panel::Panel()
    : baseClass(true)
    , m_reduced(false)
{
    //setFocusable(false);

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
    m_background.setPosition({0.f, 0.f});
    addPart(&m_background);

    // Reduced button
    m_switchReducedButton.setPosition({size().x - 60.f, 20.f});
    addPart(&m_switchReducedButton);

    // Tabs stacker
    m_tabsStacker.setAlign(nui::Stacker::CENTER);
    m_tabsStacker.setSize(0.95f * size());
    m_tabsStacker.setLocalPosition((1.f - 0.95f) * size() / 2.f);
}

//------------------------//
//----- Mouse events -----//

void Panel::handleMouseButtonPressed(const sf::Mouse::Button& mouseButton, const sf::Vector2f& mousePos)
{
    if (m_switchReducedButton.getGlobalBounds().contains(mousePos))
        switchReduced();
}

void Panel::handleMouseMoved(const sf::Vector2f& mousePos)
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

void Panel::switchReduced()
{
    setReduced(!reduced());

    if (reduced()) lerpable()->setTargetPositionOffset({0.f, size().y - 40.f});
    else lerpable()->setTargetPositionOffset({0.f, 0.f});
}
