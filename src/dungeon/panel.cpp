#include "dungeon/panel.hpp"

#include "core/application.hpp"
#include "nui/uicore.hpp"

using namespace dungeon;

Panel::Panel()
{
    setFocusable(false);
}

void Panel::init()
{
    // Background
    m_bg.setFillColor(sf::Color(0, 140, 155, 200));
    m_bg.setOutlineColor(sf::Color::White);
    m_bg.setOutlineThickness(1); // TODO Have variable

    // Tabs stacker
    core()->add(&m_tabsStacker);
    m_tabsStacker.setParent(this);
    m_tabsStacker.add(&m_monstersTab,   nui::Stacker::Align::CENTER);
    m_tabsStacker.add(&m_trapsTab,      nui::Stacker::Align::CENTER);
    m_tabsStacker.add(&m_facilitiesTab, nui::Stacker::Align::CENTER);
    m_tabsStacker.add(&m_tresuresTab,   nui::Stacker::Align::CENTER);

    // Tabs
    m_monstersTab.setText(_("Monsters"));
    m_trapsTab.setText(_("Traps"));
    m_facilitiesTab.setText(_("Facilities"));
    m_tresuresTab.setText(_("Treasures"));

    update();
}

void Panel::update()
{
    clearParts();

    // Background
    m_bg.setSize(size());
    addPart(&m_bg);

    // Tabs stacker
    m_tabsStacker.setAlign(nui::Stacker::CENTER);
    m_tabsStacker.setSize(0.95f * size());
    m_tabsStacker.setPosition((1.f - 0.95f) * size() / 2.f);

    setStatus(true);
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
}

void Panel::handleMouseMoved(const sf::Event& event, const sf::Vector2f& absPos, const sf::Vector2f& relPos)
{
}

void Panel::handleMouseLeft()
{
}

//-------------------//
//----- Changes -----//

void Panel::changedSize()
{
    update();
    baseClass::changedSize();
}
