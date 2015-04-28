#include "dungeon/sidebar.hpp"

#include "core/application.hpp"
#include "core/gettext.hpp"
#include "resources/identifiers.hpp"
#include "tools/platform-fixes.hpp" // make_unique

#include "dungeon/facilities.hpp"

using namespace dungeon;

Sidebar::Sidebar()
    : baseClass(true)
{
    // Background
    // TODO Big cheating here on rotation, better have a vertical texture too
    m_background.setTexture(&Application::context().textures.get(TextureID::DUNGEON_PANEL_BACKGROUND));
    m_background.setRotation(90);

    // Reduced button
    m_switchReducedButton.setTexture(&Application::context().textures.get(TextureID::DUNGEON_PANEL_SWITCH));
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

void Sidebar::handleMouseButtonPressed(const sf::Mouse::Button, const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    if (m_switchReducedButton.getGlobalBounds().contains(mousePos))
        switchReduced();
}

void Sidebar::handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f&)
{
    resetPartsShader();

    if (m_switchReducedButton.getGlobalBounds().contains(mousePos))
        setPartShader(&m_switchReducedButton, ShaderID::NUI_HOVER);
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

    m_tabs.clear();

    // TODO Use facilities ladder model for all.

    switch (mode) {
    case Mode::MONSTERS:
        /*m_tabs[0]->setVisual(_("Grasshopper"), TextureID::DUNGEON_MONSTERS_GRASSHOPPER, tabImageSize);
        m_tabs[1]->setVisual(_("Creepim"),     TextureID::DUNGEON_MONSTERS_CREEPIM,     tabImageSize);
        m_tabs[2]->setVisual(_("Fly-man"),     TextureID::DUNGEON_MONSTERS_FLYMAN,      tabImageSize);
        m_tabs[3]->setVisual(_("Lezard-man"),  TextureID::DUNGEON_MONSTERS_LEZARDMAN,   tabImageSize);
        m_tabs[4]->setVisual(_("Weremole"),    TextureID::DUNGEON_MONSTERS_WEREMOLE,    tabImageSize);*/
        break;

    case Mode::TRAPS:
        /*m_tabs[0]->setVisual(_("Bells"),              TextureID::DUNGEON_TRAPS_BELLS,             tabImageSize);
        m_tabs[1]->setVisual(_("Wolf trap"),          TextureID::DUNGEON_TRAPS_WOLFTRAP,          tabImageSize);
        m_tabs[2]->setVisual(_("Trompe l'oeil"),       TextureID::DUNGEON_TRAPS_TROMPELOEIL,       tabImageSize);
        m_tabs[3]->setVisual(_("Tranquilizer darts"), TextureID::DUNGEON_TRAPS_TRANQUILIZERDARTS, tabImageSize);*/
        break;

    case Mode::FACILITIES:
        m_tabs.push_back(std::make_unique<dungeon::FacilityGrabButton>(_("Treasure"),   TextureID::DUNGEON_FACILITIES_TREASURE, FacilityID::TREASURE));
        m_tabs.push_back(std::make_unique<dungeon::FacilityGrabButton>(_("Entrance"),   TextureID::DUNGEON_FACILITIES_ENTRANCE, FacilityID::ENTRANCE));
        m_tabs.push_back(std::make_unique<dungeon::FacilityGrabButton>(_("Ladder"),     TextureID::DUNGEON_FACILITIES_LADDER,   FacilityID::LADDER));

        /*m_tabs[1]->setVisual(_("Signpost"), TextureID::DUNGEON_FACILITIES_SIGNPOST, tabImageSize);
        m_tabs[1]->setVisual(_("Small chest"),   TextureID::DUNGEON_TREASURES_SMALLCHEST,   tabImageSize);
        m_tabs[2]->setVisual(_("Humble gift"),   TextureID::DUNGEON_TREASURES_HUMBLEGIFT,   tabImageSize);*/
        break;
    }

    // Add tabs to stacker
    for (auto& tab : m_tabs)
        m_tabsStacker.add(tab.get(), nui::Stacker::Align::CENTER);
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
