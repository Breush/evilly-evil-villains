#include "states/game/dungeondesign.hpp"

#include "core/gettext.hpp"
#include "core/application.hpp"
#include "states/hub/main.hpp"
#include "dungeon/detector.hpp"
#include "context/logger.hpp"
#include "context/context.hpp"
#include "context/worlds.hpp"
#include "tools/tools.hpp"
#include "tools/vector.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

using namespace states;

GameDungeonDesign::GameDungeonDesign(StateStack& stack)
    : State(stack)
    , m_dungeonInter(m_contextMenu)
    , m_dungeonSidebar(sceneGraph(), m_dungeonInter)
{
    // Creating scene
    sf::Vector2f sceneSize(1200.f, 2700.f);
    scene().setSize(sceneSize);
    auto& nuiRoot =     nuiLayer().root();
    auto& frontRoot =   scene().addLayer("FRONT",   m_depthFront).root();
    auto& closeRoot =   scene().addLayer("CLOSE",   m_depthClose).root();
    auto& dungeonRoot = scene().addLayer("DUNGEON", m_depthClose).root();
    auto& middleRoot =  scene().addLayer("MIDDLE",  m_depthMiddle).root();
    auto& farRoot =     scene().addLayer("FAR",     m_depthFar).root();
    auto& horizonRoot = scene().addLayer("HORIZON", m_depthHorizon).root();
    auto& skyRoot =     scene().addLayer("SKY",     m_depthSky).root();

    // Dungeon lighting is on
    scene().layer("DUNGEON").turnLights(true);

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
    m_loadingBackground.setTexture("core/menu/loading/background");
    m_loadingBackground.setDepth(-500.f);

    nuiRoot.attachChild(m_loadingText);
    m_loadingText.setDepth(-510.f);
    m_loadingText.setPrestyle(scene::Label::Prestyle::MENU_TITLE);
    m_loadingText.setRelativePosition({0.5f, 0.5f});
    m_loadingText.centerOrigin();
}

GameDungeonDesign::~GameDungeonDesign()
{
    // Musics
    context::context.musics.stop("core/global/musics/angevin_70");

    // Freeing resources
    Application::freeAnimations({"vanilla", "core/menu/hub"});
    Application::freeTextures({"vanilla", "core/dungeon", "core/resources", "core/tools", "core/menu/hub"});
    Application::freeSounds({"vanilla"});
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
    return baseClass::update(dt);
}

void GameDungeonDesign::refreshWindow(const config::WindowInfo& cWindow)
{
    baseClass::refreshWindow(cWindow);

    const auto& window = context::context.window;
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

//------------------//
//----- Events -----//

void GameDungeonDesign::handleEvent(const sf::Event& event)
{
    // If loading finished, wait for basic event
    if (m_loading) {
        returnif (m_loadingPercent < 100u);
        returnif (event.type != sf::Event::KeyPressed && event.type != sf::Event::MouseButtonPressed);
        closeLoadingScreen();
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

        // Go to hub
        // TODO Also have a UI element to access that
        // TODO Have that key in shortcuts config
        if (event.key.code == sf::Keyboard::H) {
            stackDynamicPush<states::HubMain>(m_dungeonData);
            return;
        }

        // Decelerate time
        if (event.key.code == sf::Keyboard::F8) {
            returnif (timeFactor() <= 0.125f);
            setTimeFactor(timeFactor() / 2.f);
            Application::visualDebug().setDisplayedTimeFactor(timeFactor());
            return;
        }

        // Accelerate time
        if (event.key.code == sf::Keyboard::F9) {
            returnif (timeFactor() >= 32.f);
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

//-----------------------//
//----- Interpreter -----//

context::CommandPtr GameDungeonDesign::interpret(std::vector<std::wstring>& tokens)
{
    returnif (!m_loading || m_loadingPercent < 100u) nullptr;

    std::wstring logMessage;
    auto nTokens = tokens.size();

    if (nTokens == 1u) {
        if (tokens[0u] == L"closeLoadingScreen") {
            logMessage = L"> [gameDungeonDesign] Closing the loading screen";
            closeLoadingScreen();
        }
    }

    if (logMessage.empty()) return nullptr;

    auto pCommand = std::make_unique<context::Command>();
    context::setCommandLog(*pCommand, logMessage);
    return std::move(pCommand);
}

void GameDungeonDesign::autoComplete(std::vector<std::wstring>& possibilities, const std::vector<std::wstring>& tokens, const std::wstring& lastToken)
{
    returnif (!m_loading || m_loadingPercent < 100u);

    auto nTokens = tokens.size();

    if (nTokens == 0u) {
        if (std::wstring(L"closeLoadingScreen").find(lastToken) == 0u)  possibilities.emplace_back(L"closeLoadingScreen");
    }
}

//-------------------//
//----- Loading -----//

void GameDungeonDesign::updateLoading(const sf::Time& dt)
{
    returnif (m_loadingPercent == 100u);

    // Think to update this...
    const uint stepsCount = 101u;
    sf::Clock clock;

    // Recurrent thingy
    #define LOAD(STEP, ...)                                                                 \
    else if (m_loadingStep == STEP) {                                                       \
        m_loadingPercent = (100u * m_loadingStep) / stepsCount;                             \
        m_loadingText.setText(_("Loading") + L"... " + toWString(m_loadingPercent) + L"%"); \
        clock.restart();                                                                    \
        __VA_ARGS__;                                                                        \
        m_loadingTime += clock.getElapsedTime().asSeconds();                                \
    }

    // Drop some update as we might have locked the application for a while
    if (m_loadingTime > 0.f) {
        m_loadingTime -= dt.asSeconds();
        return;
    }

    // Click to start screen (final step)
    if (m_loadingStep == stepsCount) {
        m_loadingPercent = 100u;
        m_loadingText.setText(_("Loading is done.\nClick to start the game."));
    }

    // Loading resources
    LOAD( 1u, Application::loadTextures({"core/tools"}))
    LOAD( 2u, Application::loadTextures({"core/resources"}))
    LOAD( 3u, Application::loadTextures({"core/dungeon/effects"}))
    LOAD( 4u, Application::loadTextures({"core/dungeon/inter"}))
    LOAD( 5u, Application::loadTextures({"core/dungeon/scene"}))
    LOAD( 6u, Application::loadTextures({"core/dungeon/sidebar"}))
    LOAD( 7u, Application::loadTextures({"vanilla/dynamics"}))
    LOAD( 8u, Application::loadTextures({"vanilla/facilities"}))
    LOAD( 9u, Application::loadTextures({"vanilla/monsters"}))
    LOAD(10u, Application::loadTextures({"vanilla/traps"}))
    LOAD(11u, Application::loadTextures({"vanilla/heroes"}))

    LOAD(15u, Application::loadSounds({"vanilla"}))

    LOAD(20u, Application::loadAnimations({"core/dungeon/effects"}))
    LOAD(21u, Application::loadAnimations({"vanilla/dynamics"}))
    LOAD(22u, Application::loadAnimations({"vanilla/facilities"}))
    LOAD(23u, Application::loadAnimations({"vanilla/monsters"}))
    LOAD(24u, Application::loadAnimations({"vanilla/traps"}))
    LOAD(25u, Application::loadAnimations({"vanilla/heroes"}))

    LOAD(30u, context::context.textures.get("core/dungeon/sidebar/tabs/monsters/cage").setRepeated(true))
    LOAD(31u, context::context.textures.get("core/dungeon/inter/outer_wall_west").setRepeated(true))
    LOAD(32u, context::context.textures.get("core/dungeon/inter/outer_wall_east").setRepeated(true))
    LOAD(33u, context::context.textures.get("core/dungeon/inter/void_room").setRepeated(true))

    // Inits
    LOAD(40u, m_dungeonInter.init())
    LOAD(41u, m_dungeonSidebar.init())

    // Dungeon inter
    LOAD(50u, m_dungeonInter.useData(m_dungeonData))
    LOAD(51u, m_dungeonInter.setRoomWidth(128.f))

    // Dungeon sidebar
    LOAD(52u, m_dungeonSidebar.useData(m_dungeonData))
    LOAD(53u, m_dungeonSidebar.setMinimapLayer(scene().layer("DUNGEON")))

    // Decorum
    LOAD(60u, m_sceneFront.setTexture("core/dungeon/scene/front"))
    LOAD(61u, m_sceneClose.setTexture("core/dungeon/scene/close"))
    LOAD(62u, m_sceneMiddle.setTexture("core/dungeon/scene/middle"))
    LOAD(63u, m_sceneFar.setTexture("core/dungeon/scene/far"))
    LOAD(64u, m_sceneHorizon.setTexture("core/dungeon/scene/horizon"))
    LOAD(65u, m_sceneSky.setTexture("core/dungeon/scene/sky"))

    // Adjust images to new maxZoom
    // TODO Sky is streched, use a setScale instead of setSize inside that function?
    LOAD(70u, scene().layer("FRONT").fitToVisibleRect(m_sceneFront))
    LOAD(71u, scene().layer("CLOSE").fitToVisibleRect(m_sceneClose))
    LOAD(72u, scene().layer("MIDDLE").fitToVisibleRect(m_sceneMiddle))
    LOAD(73u, scene().layer("FAR").fitToVisibleRect(m_sceneFar))
    LOAD(74u, scene().layer("HORIZON").fitToVisibleRect(m_sceneHorizon))
    LOAD(75u, scene().layer("SKY").fitToVisibleRect(m_sceneSky))

    // Hub
    LOAD(80u, Application::loadTextures({"core/menu/hub"}))
    LOAD(81u, Application::loadAnimations({"core/menu/hub"}))

    // Music
    // Keep last, as it is a indicator for the player that everything is ready
    LOAD(100u, context::context.musics.play("core/global/musics/angevin_70"))

    ++m_loadingStep;
}

void GameDungeonDesign::closeLoadingScreen()
{
    nuiLayer().root().detachChild(m_loadingBackground);
    nuiLayer().root().detachChild(m_loadingText);
    m_loading = false;
}

