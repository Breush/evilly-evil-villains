#include "states/game/dungeondesign.hpp"

#include "core/application.hpp"
#include "resources/identifiers.hpp"
#include "context/worlds.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

using namespace states;

GameDungeonDesign::GameDungeonDesign(StateStack& stack)
    : State(stack)
    , m_dungeonInter(m_contextMenu)
    , m_dungeonPanel(m_dungeonSidebar)
    , m_dungeonHero(&m_dungeonInter)
{
    // During game, disable key repeat
    Application::context().window.setKeyRepeatEnabled(false);

    // Stop music if any
    Application::context().musics.stop();

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
    const auto& nuiSize = nuiLayer().size();

    // Dungeon data
    massert(!context::worlds.selected().folder.empty(), "Selected world is in an empty folder.");
    m_dungeonData.load(context::worlds.selected().folder);
    m_dungeonGraph.useData(m_dungeonData);

    // Context menu
    nuiRoot.attachChild(m_contextMenu);
    m_contextMenu.setDepth(0.f);

    // Dungeon lock
    nuiRoot.attachChild(m_dungeonLock);
    m_dungeonLock.setLocalPosition(nuiSize);
    m_dungeonLock.setRelativeOrigin({1.f, 1.f});
    m_dungeonLock.useData(m_dungeonData);

    // Dungeon log
    nuiRoot.attachChild(m_dungeonLog);
    m_dungeonLog.setSize({nuiSize.x / 4.f, nuiSize.y / 2.f});
    m_dungeonLog.setLocalPosition({nuiSize.x - m_dungeonLog.size().x, 0.f});
    m_dungeonLog.setEmitter(&m_dungeonData);

    // Dungeon panel
    nuiRoot.attachChild(m_dungeonPanel);
    m_dungeonPanel.centerOrigin();
    m_dungeonPanel.setSize({4 * 100 + 25, 125 + 25});
    m_dungeonPanel.setLocalPosition({nuiSize.x / 2.f, nuiSize.y - m_dungeonPanel.size().y / 2.f});
    m_dungeonPanel.lerpable()->saveDefaults();
    m_dungeonPanel.setEmitter(&m_dungeonData);

    // Dungeon sidebar
    nuiRoot.attachChild(m_dungeonSidebar);
    m_dungeonSidebar.centerOrigin();
    m_dungeonSidebar.setSize({2 * 100 + 25, 5 * 125 + 25});
    m_dungeonSidebar.setLocalPosition({nuiSize.x - m_dungeonSidebar.size().x / 2.f, nuiSize.y / 2.f});
    m_dungeonSidebar.lerpable()->saveDefaults();
    m_dungeonSidebar.immediateReduce();
    m_dungeonSidebar.setEmitter(&m_dungeonData);

    // Dungeon summary
    nuiRoot.attachChild(m_dungeonSummary);
    m_dungeonSummary.useData(m_dungeonData);

    // Dungeon inter
    dungeonRoot.attachChild(m_dungeonInter);
    m_dungeonInter.useData(m_dungeonData);
    // TODO No setSize here...
    m_dungeonInter.setSize({700.f, 900.f});
    m_dungeonInter.setLocalPosition({sceneSize.x / 2.f - 350.f, sceneSize.y - 950.f});

    // Dungeon hero
    // TODO Have a Hero manager or so (Hero then, should not be a receiver?)
    m_dungeonInter.attachChild(m_dungeonHero);
    m_dungeonHero.useGraph(m_dungeonGraph);
    m_dungeonHero.useData(m_dungeonData);

    // Decorum
    frontRoot.attachChild(m_sceneFront);
    closeRoot.attachChild(m_sceneClose);
    middleRoot.attachChild(m_sceneMiddle);
    farRoot.attachChild(m_sceneFar);
    horizonRoot.attachChild(m_sceneHorizon);
    skyRoot.attachChild(m_sceneSky);

    m_sceneFront.setTexture(TextureID::DUNGEON_SCENE_FRONT);
    m_sceneClose.setTexture(TextureID::DUNGEON_SCENE_CLOSE);
    m_sceneMiddle.setTexture(TextureID::DUNGEON_SCENE_MIDDLE);
    m_sceneFar.setTexture(TextureID::DUNGEON_SCENE_FAR);
    m_sceneHorizon.setTexture(TextureID::DUNGEON_SCENE_HORIZON);
    m_sceneSky.setTexture(TextureID::DUNGEON_SCENE_SKY);

    // Adjust images to new maxZoom
    // TODO Sky is streched, use a setScale instead of setSize inside that function?
    scene().layer("FRONT").fitToVisibleRect(m_sceneFront);
    scene().layer("CLOSE").fitToVisibleRect(m_sceneClose);
    scene().layer("MIDDLE").fitToVisibleRect(m_sceneMiddle);
    scene().layer("FAR").fitToVisibleRect(m_sceneFar);
    scene().layer("HORIZON").fitToVisibleRect(m_sceneHorizon);
    scene().layer("SKY").fitToVisibleRect(m_sceneSky);
    scene().centerRelative({0.5f, 1.f});
}

void GameDungeonDesign::onQuit() noexcept
{
    // Saving dungeon + villain info
    context::worlds.refreshLastPlayed();
    m_dungeonData.save(context::worlds.selected().folder);
}

bool GameDungeonDesign::handleEvent(const sf::Event& event)
{
    // Global events
    m_dungeonInter.handleGlobalEvent(event);
    m_contextMenu.handleGlobalEvent(event);

    if (event.type == sf::Event::KeyPressed) {
        // Escape pressed, trigger the pause screen
        if (event.key.code == sf::Keyboard::Escape) {
            stackPush(StateID::GAME_PAUSE);
        }
    }

    return State::handleEvent(event);
}

