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
    sf::Vector2f sceneSize(1024.f, 768.f);
    scene().setSize(sceneSize);
    auto& nuiRoot = nuiLayer().root();
    auto& backgroundRoot =  scene().addLayer("BACKGROUND", 1.2f).root();
    auto& dungeonRoot =     scene().addLayer("DUNGEON",    1.f).root();
    auto& foregroundRoot =  scene().addLayer("FOREGROUND", 0.8f).root();
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
    m_dungeonInter.setSize({350.f, 450.f});
    m_dungeonInter.setLocalPosition({sceneSize.x / 3.f, sceneSize.y - 450.f - 100.f});

    // Dungeon hero
    // TODO Have a Hero manager or so (Hero then, should not be a receiver?)
    m_dungeonInter.attachChild(m_dungeonHero);
    m_dungeonHero.useGraph(m_dungeonGraph);
    m_dungeonHero.useData(m_dungeonData);

    // Decorum
    backgroundRoot.attachChild(m_decorumBack);
    m_decorumBack.setTexture(TextureID::DUNGEON_SCENE_GRASSYHILLS_BACK);

    dungeonRoot.attachChild(m_decorumMiddle);
    m_decorumMiddle.setTexture(TextureID::DUNGEON_SCENE_GRASSYHILLS_MIDDLE);
    m_decorumMiddle.setDepth(100.f);

    foregroundRoot.attachChild(m_decorumFront);
    m_decorumFront.setTexture(TextureID::DUNGEON_SCENE_GRASSYHILLS_FRONT);
}

void GameDungeonDesign::onQuit() noexcept
{
    // Saving dungeon
    m_dungeonData.save(context::worlds.selected().folder);

    // Saving worlds info
    context::worlds.refreshLastPlayed();
    context::worlds.save();
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

