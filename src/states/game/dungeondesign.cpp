#include "states/game/dungeondesign.hpp"

#include "core/application.hpp"
#include "context/worlds.hpp"
#include "tools/tools.hpp"
#include "tools/vector.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

using namespace states;

GameDungeonDesign::GameDungeonDesign(StateStack& stack)
    : State(stack)
    , m_dungeonInter(m_contextMenu)
    , m_dungeonSidebar(scene())
    , m_heroesManager(m_dungeonInter)
{
    // Loading resources
    Application::loadTextures({"dungeon", "elements", "heroes"});
    Application::loadAnimations({"dungeon", "heroes"});

    Application::context().textures.get("dungeon/sidebar/tab/monsters/cage").setRepeated(true);
    Application::context().textures.get("dungeon/inter/outer_wall_west").setRepeated(true);
    Application::context().textures.get("dungeon/inter/outer_wall_east").setRepeated(true);

    // Stop music if any
    Application::context().musics.stop();

    // Inits
    m_dungeonInter.init();
    m_dungeonSidebar.init();

    // Creating scene
    sf::Vector2f sceneSize(1200.f, 2700.f);
    scene().setSize(sceneSize);
    auto& nuiRoot = nuiLayer().root();
    auto& frontRoot =   scene().addLayer("FRONT",   m_depthFront).root();
    auto& closeRoot =   scene().addLayer("CLOSE",   m_depthClose).root();
    auto& dungeonRoot = scene().addLayer("DUNGEON", m_depthClose).root();
    auto& middleRoot =  scene().addLayer("MIDDLE",  m_depthMiddle).root();
    auto& farRoot =     scene().addLayer("FAR",     m_depthFar).root();
    auto& horizonRoot = scene().addLayer("HORIZON", m_depthHorizon).root();
    auto& skyRoot =     scene().addLayer("SKY",     m_depthSky).root();

    // Dungeon data
    massert(!context::worlds.selected().folder.empty(), "Selected world is in an empty folder.");
    m_dungeonData.load(context::worlds.selected().folder);
    m_dungeonGraph.useData(m_dungeonData);

    // Context menu
    nuiRoot.attachChild(m_contextMenu);
    m_contextMenu.setRelativeOrigin({0.5f, 0.05f});
    m_contextMenu.setDepth(-1.f);

    // Dungeon sidebar
    nuiRoot.attachChild(m_dungeonSidebar);
    m_dungeonSidebar.setRelativeOrigin({1.f, 0.f});
    m_dungeonSidebar.useData(m_dungeonData);
    m_dungeonSidebar.setMinimapLayer(scene().layer("DUNGEON"));

    // Dungeon inter
    dungeonRoot.attachChild(m_dungeonInter);
    m_dungeonInter.useData(m_dungeonData);
    m_dungeonInter.setRoomWidth(128.f);
    m_dungeonInter.setLocalPosition({120.f, sceneSize.y - 50.f});
    m_dungeonInter.setRelativeOrigin({0.f, 1.f});

    // Dungeon hero
    m_heroesManager.useGraph(m_dungeonGraph);
    m_heroesManager.useData(m_dungeonData);

    // Decorum
    frontRoot.attachChild(m_sceneFront);
    closeRoot.attachChild(m_sceneClose);
    middleRoot.attachChild(m_sceneMiddle);
    farRoot.attachChild(m_sceneFar);
    horizonRoot.attachChild(m_sceneHorizon);
    skyRoot.attachChild(m_sceneSky);

    m_sceneFront.setTexture("dungeon/scene/front");
    m_sceneClose.setTexture("dungeon/scene/close");
    m_sceneMiddle.setTexture("dungeon/scene/middle");
    m_sceneFar.setTexture("dungeon/scene/far");
    m_sceneHorizon.setTexture("dungeon/scene/horizon");
    m_sceneSky.setTexture("dungeon/scene/sky");

    // Adjust images to new maxZoom
    // TODO Sky is streched, use a setScale instead of setSize inside that function?
    scene().layer("FRONT").fitToVisibleRect(m_sceneFront);
    scene().layer("CLOSE").fitToVisibleRect(m_sceneClose);
    scene().layer("MIDDLE").fitToVisibleRect(m_sceneMiddle);
    scene().layer("FAR").fitToVisibleRect(m_sceneFar);
    scene().layer("HORIZON").fitToVisibleRect(m_sceneHorizon);
    scene().layer("SKY").fitToVisibleRect(m_sceneSky);
}

GameDungeonDesign::~GameDungeonDesign()
{
    // Freeing resources
    Application::freeTextures({"dungeon", "elements", "heroes"});
    Application::freeAnimations({"dungeon", "heroes"});
}

//-------------------//
//----- Routine -----//

bool GameDungeonDesign::update(const sf::Time& dt)
{
    m_heroesManager.update(dt);
    return baseClass::update(dt);
}

void GameDungeonDesign::refreshWindow(const config::WindowInfo& cWindow)
{
    baseClass::refreshWindow(cWindow);

    const auto& window = Application::context().window;
    const auto& screenSize = cWindow.screenSize;
    const auto& resolution = cWindow.resolution;

    // Re-adjust, so that it does not get out of screen
    // TODO Have relativeKeepInside?
    m_contextMenu.setKeepInsideLocalRect({0.f, 0.f, resolution.x, resolution.y});

    // Sidebar
    const float sidebarWidth = 200.f;
    m_dungeonSidebar.setSize({sidebarWidth, resolution.y});
    m_dungeonSidebar.setLocalPosition({resolution.x, 0.f});

    // Scene viewport
    const sf::FloatRect sceneRect{0.f, 0.f, resolution.x - sidebarWidth, resolution.y};
    auto sceneScreenRect = tools::mapRectCoordsToPixel(window, sceneRect, &nuiLayer().view());
    scene().setViewport(sceneScreenRect / screenSize);

    // Center view
    scene().centerRelative({0.5f, 1.f});
}

void GameDungeonDesign::onQuit() noexcept
{
    // Saving dungeon + villain info
    context::worlds.refreshLastPlayed();
    m_dungeonData.save(context::worlds.selected().folder);
}

//-------------------//
//----- Routine -----//

void GameDungeonDesign::handleEvent(const sf::Event& event)
{
    // Global events
    m_dungeonInter.handleGlobalEvent(event);
    m_contextMenu.handleGlobalEvent(event);

    if (event.type == sf::Event::KeyPressed
        && event.key.code == sf::Keyboard::Escape) {
        stackPush(StateID::GAME_PAUSE);
        return;
    }

    State::handleEvent(event);
}

