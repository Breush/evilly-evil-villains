#include "dungeon/panel.hpp"

#include "core/application.hpp"
#include "tools/math.hpp"
#include "nui/uicore.hpp"

using namespace dungeon;

Panel::Panel()
    : m_reducedSpeedBySecond(250.f) // TODO Defined somewhere in config?
    , m_reduced(true)
{
    setFocusable(false);
}

void Panel::init()
{
    // Background
    m_background.setTexture(&Application::context().textures.get(Textures::DUNGEON_PANEL_BACKGROUND));

    // Reduced button
    m_switchReducedButton.setTexture(&Application::context().textures.get(Textures::DUNGEON_PANEL_SWITCH));
    m_switchReducedButton.setSize({20.f, 20.f});

    // Tabs stacker
    core()->add(&m_tabsStacker);
    m_tabsStacker.setParent(this);

    // TODO Make a std::array to store tabs
    m_monstersTab.setZDepth(zDepth() - 1);
    m_trapsTab.setZDepth(zDepth() - 1);
    m_facilitiesTab.setZDepth(zDepth() - 1);
    m_treasuresTab.setZDepth(zDepth() - 1);

    m_tabsStacker.add(&m_monstersTab,   nui::Stacker::Align::CENTER);
    m_tabsStacker.add(&m_trapsTab,      nui::Stacker::Align::CENTER);
    m_tabsStacker.add(&m_facilitiesTab, nui::Stacker::Align::CENTER);
    m_tabsStacker.add(&m_treasuresTab,  nui::Stacker::Align::CENTER);

    // Tabs
    // TODO Get display size from somewhere
    m_monstersTab.setVisual(_("Monsters"), Textures::DUNGEON_PANEL_MONSTERS, {80.f, 80.f});
    m_trapsTab.setVisual(_("Traps"), Textures::DUNGEON_PANEL_TRAPS, {80.f, 80.f});
    m_facilitiesTab.setVisual(_("Facilities"), Textures::DUNGEON_PANEL_FACILITIES, {80.f, 80.f});
    m_treasuresTab.setVisual(_("Treasures"), Textures::DUNGEON_PANEL_TREASURES, {80.f, 80.f});
    m_tabsStacker.update();

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
    m_tabsStacker.setLocalPosition((1.f - 0.95f) * size() / 2.f, false);

    // Reduced
    // Set target with the new parameters
    if (reduced()) setReducedVector({0.f, size().y - 40.f});
    else setReducedVector({0.f, 0.f});

    setStatus(true);
}

void Panel::update(const sf::Time& dt)
{
    // Have it encasulated inside a Animation class
    float x = 0.f, y = 0.f;

    // Animation of reduced vector
    if (m_reducedVectorAnimation.y < m_reducedVector.y) {
        y = m_reducedSpeedBySecond * dt.asSeconds();
        if (m_reducedVectorAnimation.y + y > m_reducedVector.y)
            y = m_reducedVector.y - m_reducedVectorAnimation.y;
    }
    else if (m_reducedVectorAnimation.y > m_reducedVector.y) {
        y -= m_reducedSpeedBySecond * dt.asSeconds();
        if (m_reducedVectorAnimation.y + y < m_reducedVector.y)
            y = m_reducedVector.y - m_reducedVectorAnimation.y;
    }

    // Animate
    if (x != 0.f || y != 0.f) {
        m_reducedVectorAnimation += sf::Vector2f(x, y);
        localMove({x, y});
    }
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
}

void Panel::handleMouseLeft()
{
}

//------------------------//
//----- Reduced mode -----//

void Panel::switchReduced()
{
    setReduced(!reduced());
}

//-------------------//
//----- Changes -----//

void Panel::changedStatus()
{
    m_tabsStacker.parentStatusChanged();
}

void Panel::changedSize()
{
    update();
    baseClass::changedSize();
}

void Panel::changedReduced()
{
    update();
}
