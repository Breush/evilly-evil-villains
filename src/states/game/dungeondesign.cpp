#include "states/game/dungeondesign.hpp"

#include "core/application.hpp"
#include "dungeon/detector.hpp"
#include "context/worlds.hpp"
#include "tools/tools.hpp"
#include "tools/vector.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

using namespace states;

GameDungeonDesign::GameDungeonDesign(StateStack& stack)
    : State(stack)
    , m_dungeonInter(m_contextMenu, m_heroesManager)
    , m_dungeonSidebar(scene(), m_dungeonInter, m_dungeonData)
    , m_heroesManager(m_dungeonInter)
{
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
    m_dungeonData.load(context::worlds.selected().folder);
    m_dungeonGraph.useData(m_dungeonData);

    // Context menu
    nuiRoot.attachChild(m_contextMenu);
    m_contextMenu.setRelativeOrigin({0.5f, 0.05f});
    m_contextMenu.setDepth(-1.f);

    // Dungeon sidebar
    nuiRoot.attachChild(m_dungeonSidebar);
    m_dungeonSidebar.setRelativeOrigin({1.f, 0.f});

    // Dungeon inter
    dungeonRoot.attachChild(m_dungeonInter);
    m_dungeonInter.setLocalPosition({120.f, sceneSize.y - 50.f});
    m_dungeonInter.setRelativeOrigin({0.f, 1.f});

    // Decorum
    frontRoot.attachChild(m_sceneFront);
    closeRoot.attachChild(m_sceneClose);
    middleRoot.attachChild(m_sceneMiddle);
    farRoot.attachChild(m_sceneFar);
    horizonRoot.attachChild(m_sceneHorizon);
    skyRoot.attachChild(m_sceneSky);
}

GameDungeonDesign::~GameDungeonDesign()
{
    // Musics
    Application::context().musics.stop("angevin_70");

    // Freeing resources
    Application::freeTextures({"dungeon", "elements", "heroes", "effects"});
    Application::freeAnimations({"dungeon", "heroes"});
}

//-------------------//
//----- Routine -----//

bool GameDungeonDesign::update(const sf::Time& dt)
{
    // Continue loading the state if still in this phase
    if (m_loading) {
        updateLoading(dt);
        return true;
    }

    // Loading is over, we're just updating the whole thing
    auto dtFactored = timeFactor() * dt;
    m_dungeonData.update(dtFactored);
    m_heroesManager.update(dtFactored);
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
    const float sidebarWidth = 250.f;
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
    Application::visualDebug().setDisplayedTimeFactor(1.f);
}

//-------------------//
//----- Loading -----//

void GameDungeonDesign::updateLoading(const sf::Time& dt)
{
    // TODO Update a loading text percent
    // and have an overlay image during that loading process
    #define LOAD(STEP, ...)             \
    if (m_loadingStep == STEP) {        \
        ++m_loadingStep;                \
        __VA_ARGS__;                    \
        return;                         \
    }

    // Music
    LOAD( 0u, Application::context().musics.play("angevin_70"));

    // Loading resources
    LOAD( 1u, Application::loadTextures({"dungeon/facilities"}));
    LOAD( 2u, Application::loadTextures({"dungeon/inter"}));
    LOAD( 3u, Application::loadTextures({"dungeon/monsters"}));
    LOAD( 4u, Application::loadTextures({"dungeon/panel"}));
    LOAD( 5u, Application::loadTextures({"dungeon/scene"}));
    LOAD( 6u, Application::loadTextures({"dungeon/sidebar"}));
    LOAD( 7u, Application::loadTextures({"dungeon/tools"}));
    LOAD( 8u, Application::loadTextures({"dungeon/traps"}));
    LOAD( 9u, Application::loadTextures({"elements"}));
    LOAD(10u, Application::loadTextures({"heroes"}));
    LOAD(11u, Application::loadTextures({"effects"}));

    LOAD(12u, Application::loadAnimations({"dungeon/facilities"}));
    LOAD(13u, Application::loadAnimations({"dungeon/monsters"}));
    LOAD(14u, Application::loadAnimations({"dungeon/traps"}));
    LOAD(15u, Application::loadAnimations({"heroes"}));

    LOAD(16u, Application::context().textures.get("dungeon/sidebar/tab/monsters/cage").setRepeated(true));
    LOAD(17u, Application::context().textures.get("dungeon/inter/outer_wall_west").setRepeated(true));
    LOAD(18u, Application::context().textures.get("dungeon/inter/outer_wall_east").setRepeated(true));

    // Inits
    LOAD(19u, m_dungeonInter.init());
    LOAD(20u, m_dungeonSidebar.init());

    // Dungeon sidebar
    LOAD(21u, m_dungeonSidebar.useData(m_dungeonData));
    LOAD(22u, m_dungeonSidebar.setMinimapLayer(scene().layer("DUNGEON")));

    // Dungeon inter
    LOAD(23u, m_dungeonInter.useData(m_dungeonData));
    LOAD(24u, m_dungeonInter.setRoomWidth(128.f));

    // Dungeon hero
    LOAD(25u, m_heroesManager.useGraph(m_dungeonGraph));
    LOAD(26u, m_heroesManager.useData(m_dungeonData));

    // Decorum
    LOAD(27u, m_sceneFront.setTexture("dungeon/scene/front"));
    LOAD(28u, m_sceneClose.setTexture("dungeon/scene/close"));
    LOAD(29u, m_sceneMiddle.setTexture("dungeon/scene/middle"));
    LOAD(30u, m_sceneFar.setTexture("dungeon/scene/far"));
    LOAD(31u, m_sceneHorizon.setTexture("dungeon/scene/horizon"));
    LOAD(32u, m_sceneSky.setTexture("dungeon/scene/sky"));

    // Adjust images to new maxZoom
    // TODO Sky is streched, use a setScale instead of setSize inside that function?
    LOAD(33u, scene().layer("FRONT").fitToVisibleRect(m_sceneFront));
    LOAD(34u, scene().layer("CLOSE").fitToVisibleRect(m_sceneClose));
    LOAD(35u, scene().layer("MIDDLE").fitToVisibleRect(m_sceneMiddle));
    LOAD(36u, scene().layer("FAR").fitToVisibleRect(m_sceneFar));
    LOAD(37u, scene().layer("HORIZON").fitToVisibleRect(m_sceneHorizon));
    LOAD(38u, scene().layer("SKY").fitToVisibleRect(m_sceneSky));

    // TODO Have a "Click to start" screen.
    // That could replace that extra delay

    // Padding an extra delay to absorb possible dt rush
    m_loadingTime += dt.asSeconds();
    if (m_loadingTime > 2.f)
        m_loading = false;
}

//------------------//
//----- Events -----//

void GameDungeonDesign::handleEvent(const sf::Event& event)
{
    // Global events
    m_dungeonInter.handleGlobalEvent(event);
    m_contextMenu.handleGlobalEvent(event);

    if (event.type == sf::Event::KeyPressed) {
        // Pause on escape
        if (event.key.code == sf::Keyboard::Escape) {
            if (!isStateVisible(StateID::GAME_PAUSE))
                stackPush(StateID::GAME_PAUSE);
            return;
        }

        // Decelerate time
        if (event.key.code == sf::Keyboard::F8) {
            setTimeFactor(timeFactor() / 2.f);
            Application::visualDebug().setDisplayedTimeFactor(timeFactor());
            return;
        }

        // Accelerate time
        if (event.key.code == sf::Keyboard::F9) {
            setTimeFactor(timeFactor() * 2.f);
            Application::visualDebug().setDisplayedTimeFactor(timeFactor());
            return;
        }
    }

    // TODO Make it a config option?
    #if DEBUG_GLOBAL == 0
    if (event.type == sf::Event::LostFocus) {
        if (!isStateVisible(StateID::GAME_PAUSE))
            stackPush(StateID::GAME_PAUSE);
        return;
    }
    #endif

    State::handleEvent(event);
}

