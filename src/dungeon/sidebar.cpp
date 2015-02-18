#include "dungeon/sidebar.hpp"

#include "core/application.hpp"

using namespace dungeon;

Sidebar::Sidebar()
    : baseClass(true)
{
    // Background
    // TODO Big cheating here on rotation, better have a vertical texture too
    m_background.setTexture(&Application::context().textures.get(Textures::DUNGEON_PANEL_BACKGROUND));
    m_background.setRotation(90);

    // Reduced button
    m_switchReducedButton.setTexture(&Application::context().textures.get(Textures::DUNGEON_PANEL_SWITCH));
    m_switchReducedButton.setPosition({45.f, 25.f});
    m_switchReducedButton.setSize({20.f, 20.f});
    m_switchReducedButton.setRotation(90);

    // Tabs
    attachChild(m_tabsStacker);
    m_tabsStacker.setCentered(true);

    update();
}

//-------------------//
//----- Routine -----//

void Sidebar::update()
{
    clearParts();

    // Background
    m_background.setPosition({size().x, 0.f});
    m_background.setSize({size().y, size().x});
    addPart(&m_background);

    // Reduced button
    addPart(&m_switchReducedButton);

    // Tabs
    m_tabsStacker.setSize(0.9f * size());
    m_tabsStacker.setLocalPosition(size() / 2.f);
}

//------------------------//
//----- Mouse events -----//

void Sidebar::handleMouseButtonPressed(const sf::Mouse::Button&, const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    if (m_switchReducedButton.getGlobalBounds().contains(mousePos))
        switchReduced();
}

void Sidebar::handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    resetPartsShader();

    if (m_switchReducedButton.getGlobalBounds().contains(mousePos))
        setPartShader(&m_switchReducedButton, Shaders::NUI_HOVER);
}

void Sidebar::handleMouseLeft()
{
    resetPartsShader();
}

//----------------//
//----- Mode -----//

void Sidebar::setMode(Mode mode)
{
    m_tabsStacker.clear();

    // TODO Get tabImageSize from somewhere or directly stored in imageButton?
    sf::Vector2f tabImageSize(80.f, 80.f);

    // TODO Think about a better way to do this (and save it somewhere)
    // TODO Get tabs from dungeon::Data?
    // TODO Set callbacks
    if (mode == Mode::MONSTERS) {
        m_tabs.resize(5);
        m_tabs[0].setVisual(_("Grasshopper"), Textures::DUNGEON_MONSTERS_GRASSHOPPER, tabImageSize);
        m_tabs[1].setVisual(_("Creepim"),     Textures::DUNGEON_MONSTERS_CREEPIM,     tabImageSize);
        m_tabs[2].setVisual(_("Fly-man"),     Textures::DUNGEON_MONSTERS_FLYMAN,      tabImageSize);
        m_tabs[3].setVisual(_("Lezard-man"),  Textures::DUNGEON_MONSTERS_LEZARDMAN,   tabImageSize);
        m_tabs[4].setVisual(_("Weremole"),    Textures::DUNGEON_MONSTERS_WEREMOLE,    tabImageSize);
    }
    else if (mode == Mode::TRAPS) {
        m_tabs.resize(4);
        m_tabs[0].setVisual(_("Bells"),              Textures::DUNGEON_TRAPS_BELLS,             tabImageSize);
        m_tabs[1].setVisual(_("Wolf trap"),          Textures::DUNGEON_TRAPS_WOLFTRAP,          tabImageSize);
        m_tabs[2].setVisual(_("Trompe l'oeil"),       Textures::DUNGEON_TRAPS_TROMPELOEIL,       tabImageSize);
        m_tabs[3].setVisual(_("Tranquilizer darts"), Textures::DUNGEON_TRAPS_TRANQUILIZERDARTS, tabImageSize);
    }
    else if (mode == Mode::FACILITIES) {
        m_tabs.resize(2);
        m_tabs[0].setVisual(_("Ladder"),   Textures::DUNGEON_FACILITIES_LADDER,   tabImageSize);
        m_tabs[1].setVisual(_("Signpost"), Textures::DUNGEON_FACILITIES_SIGNPOST, tabImageSize);
    }
    else if (mode == Mode::TREASURES) {
        m_tabs.resize(3);
        m_tabs[0].setVisual(_("Treasure room"), Textures::DUNGEON_TREASURES_TREASUREROOM, tabImageSize);
        m_tabs[1].setVisual(_("Small chest"),   Textures::DUNGEON_TREASURES_SMALLCHEST,   tabImageSize);
        m_tabs[2].setVisual(_("Humble gift"),   Textures::DUNGEON_TREASURES_HUMBLEGIFT,   tabImageSize);
    }

    for (auto& tab : m_tabs)
        m_tabsStacker.add(&tab, nui::Stacker::Align::CENTER);
}

//------------------------//
//----- Reduced mode -----//

void Sidebar::setReduced(bool reduced)
{
    m_reduced = reduced;

    if (m_reduced) lerpable()->setTargetPositionOffset({size().x - 45.f,0.f});
    else lerpable()->setTargetPositionOffset({0.f, 0.f});
}

void Sidebar::switchReduced()
{
    setReduced(!m_reduced);
}

void Sidebar::immediateReduce()
{
    setReduced(true);
    setLocalPosition(lerpable()->targetPosition());
}
