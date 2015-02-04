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
    m_bg.setFillColor(sf::Color(0, 140, 155, 200));
    m_bg.setOutlineColor(sf::Color::White);
    m_bg.setOutlineThickness(1); // TODO Have variable

    // Reduced button
    m_switchReducedButton.setFillColor(sf::Color(150, 140, 10, 255));
    m_switchReducedButton.setOutlineColor(sf::Color::White);
    m_switchReducedButton.setOutlineThickness(1);
    m_switchReducedButton.setSize({20.f, 20.f});

    // Tabs stacker
    core()->add(&m_tabsStacker);
    m_tabsStacker.setParent(this);
    m_tabsStacker.add(&m_monstersTab,   nui::Stacker::Align::CENTER);
    m_tabsStacker.add(&m_trapsTab,      nui::Stacker::Align::CENTER);
    m_tabsStacker.add(&m_facilitiesTab, nui::Stacker::Align::CENTER);
    m_tabsStacker.add(&m_treasuresTab,   nui::Stacker::Align::CENTER);

    // Tabs
    // TODO Get display size from somewhere
    m_monstersTab.setVisual(_("Monsters"), Textures::DUNGEON_PANEL_MONSTERS, {80.f, 80.f});
    m_trapsTab.setVisual(_("Traps"), Textures::DUNGEON_PANEL_TRAPS, {80.f, 80.f});
    m_facilitiesTab.setVisual(_("Facilities"), Textures::DUNGEON_PANEL_FACILITIES, {80.f, 80.f});
    m_treasuresTab.setVisual(_("Treasures"), Textures::DUNGEON_PANEL_TREASURES, {80.f, 80.f});

    update();
}

void Panel::update()
{
    clearParts();

    // Background
    m_bg.setSize(size());
    m_bg.setPosition({0.f, 0.f});
    addPart(&m_bg);

    // Reduced button
    m_switchReducedButton.setPosition({size().x - 20.f, 0.f});
    addPart(&m_switchReducedButton);

    // Tabs stacker
    m_tabsStacker.setAlign(nui::Stacker::CENTER);
    m_tabsStacker.setSize(0.95f * size());
    m_tabsStacker.setPosition((1.f - 0.95f) * size() / 2.f);

    // Reduced
    // Set target with the new parameters
    if (reduced()) setReducedVector({0.f, size().y - 20.f});
    else setReducedVector({0.f, 0.f});

    m_tabsStacker.update();
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
        setStatus(true);
        move({x, y});
    }
}

//------------------------//
//----- Mouse events -----//

void Panel::handleMouseEvent(const sf::Event& event, const sf::Vector2f& absPos, const sf::Vector2f& relPos)
{
    switch (event.type) {
    case sf::Event::MouseButtonPressed:
        handleMousePressed(event, absPos, relPos);
        break;
    case sf::Event::MouseMoved:
        handleMouseMoved(event, absPos, relPos);
        break;
    case sf::Event::MouseLeft:
        handleMouseLeft();
        break;
    default:
        break;
    }
}

void Panel::handleMousePressed(const sf::Event& event, const sf::Vector2f& absPos, const sf::Vector2f& relPos)
{
    if (isInsideRect(relPos, m_switchReducedButton.getGlobalBounds()))
        switchReduced();
}

void Panel::handleMouseMoved(const sf::Event& event, const sf::Vector2f& absPos, const sf::Vector2f& relPos)
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
    m_tabsStacker.setStatus(true);
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
