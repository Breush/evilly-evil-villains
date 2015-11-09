#include "states/game/dungeondesign.hpp"

#include "core/gettext.hpp"
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

    // Loading
    nuiRoot.attachChild(m_loadingBackground);
    m_loadingBackground.setDepth(-500.f);
    m_loadingBackground.setFillColor(sf::Color::Black);

    nuiRoot.attachChild(m_loadingText);
    m_loadingText.setDepth(-510.f);
    m_loadingText.setPrestyle(scene::Label::Prestyle::MENU_TITLE);
    m_loadingText.setRelativePosition({0.5f, 0.5f});
    m_loadingText.centerOrigin();
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

    // Loading
    m_loadingBackground.setSize(resolution);

    // Re-adjust, so that it does not get out of screen
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
    // Think to update this...
    const uint stepsCount = 39u;

    // Recurrent thingy
    #define LOAD(STEP, ...)                                                                 \
    if (m_loadingStep == STEP) {                                                            \
        ++m_loadingStep;                                                                    \
        m_loadingPercent = (100u * m_loadingStep) / stepsCount;                             \
        m_loadingText.setText(_("Loading") + L"... " + toWString(m_loadingPercent) + L"%"); \
        __VA_ARGS__;                                                                        \
        return;                                                                             \
    }

    // Loading resources
    LOAD( 0u, Application::loadTextures({"dungeon/facilities"}));
    LOAD( 1u, Application::loadTextures({"dungeon/inter"}));
    LOAD( 2u, Application::loadTextures({"dungeon/monsters"}));
    LOAD( 3u, Application::loadTextures({"dungeon/panel"}));
    LOAD( 4u, Application::loadTextures({"dungeon/scene"}));
    LOAD( 5u, Application::loadTextures({"dungeon/sidebar"}));
    LOAD( 6u, Application::loadTextures({"dungeon/tools"}));
    LOAD( 7u, Application::loadTextures({"dungeon/traps"}));
    LOAD( 8u, Application::loadTextures({"elements"}));
    LOAD( 9u, Application::loadTextures({"heroes"}));
    LOAD(10u, Application::loadTextures({"effects"}));

    LOAD(11u, Application::loadAnimations({"dungeon/facilities"}));
    LOAD(12u, Application::loadAnimations({"dungeon/monsters"}));
    LOAD(13u, Application::loadAnimations({"dungeon/traps"}));
    LOAD(14u, Application::loadAnimations({"heroes"}));

    LOAD(15u, Application::context().textures.get("dungeon/sidebar/tab/monsters/cage").setRepeated(true));
    LOAD(16u, Application::context().textures.get("dungeon/inter/outer_wall_west").setRepeated(true));
    LOAD(17u, Application::context().textures.get("dungeon/inter/outer_wall_east").setRepeated(true));

    // Inits
    LOAD(18u, m_dungeonInter.init());
    LOAD(19u, m_dungeonSidebar.init());

    // Dungeon inter
    LOAD(20u, m_dungeonInter.useData(m_dungeonData));
    LOAD(21u, m_dungeonInter.setRoomWidth(128.f));

    // Dungeon sidebar
    LOAD(22u, m_dungeonSidebar.useData(m_dungeonData));
    LOAD(23u, m_dungeonSidebar.setMinimapLayer(scene().layer("DUNGEON")));

    // Dungeon hero
    LOAD(24u, m_heroesManager.useGraph(m_dungeonGraph));
    LOAD(25u, m_heroesManager.useData(m_dungeonData));

    // Decorum
    LOAD(26u, m_sceneFront.setTexture("dungeon/scene/front"));
    LOAD(27u, m_sceneClose.setTexture("dungeon/scene/close"));
    LOAD(28u, m_sceneMiddle.setTexture("dungeon/scene/middle"));
    LOAD(29u, m_sceneFar.setTexture("dungeon/scene/far"));
    LOAD(30u, m_sceneHorizon.setTexture("dungeon/scene/horizon"));
    LOAD(31u, m_sceneSky.setTexture("dungeon/scene/sky"));

    // Adjust images to new maxZoom
    // TODO Sky is streched, use a setScale instead of setSize inside that function?
    LOAD(32u, scene().layer("FRONT").fitToVisibleRect(m_sceneFront));
    LOAD(33u, scene().layer("CLOSE").fitToVisibleRect(m_sceneClose));
    LOAD(34u, scene().layer("MIDDLE").fitToVisibleRect(m_sceneMiddle));
    LOAD(35u, scene().layer("FAR").fitToVisibleRect(m_sceneFar));
    LOAD(36u, scene().layer("HORIZON").fitToVisibleRect(m_sceneHorizon));
    LOAD(37u, scene().layer("SKY").fitToVisibleRect(m_sceneSky));

    // Music
    // Keep last, as it is a indicator for the player that everything is ready
    LOAD(38u, Application::context().musics.play("angevin_70"));

    // Click to start screen
    m_loadingText.setText(_("Loading is done.\nClick to start the game."));
}

//------------------//
//----- Events -----//

void GameDungeonDesign::handleEvent(const sf::Event& event)
{
    // If loading finished, wait for basic event
    if (m_loading) {
        returnif (m_loadingPercent < 100u);
        returnif (event.type != sf::Event::KeyPressed && event.type != sf::Event::MouseButtonPressed);
        nuiLayer().root().detachChild(m_loadingBackground);
        nuiLayer().root().detachChild(m_loadingText);
        m_loading = false;
    }

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

