#include "states/game/dungeondesign.hpp"

#include "core/application.hpp"
#include "resources/identifiers.hpp"
#include "world/context.hpp"

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
    massert(!world::context.selectedWorld().folder.empty(), "Selected world is in an empty folder.");
    m_dungeonData.load(world::context.selectedWorld().folder);
    m_dungeonGraph.useData(m_dungeonData);

    // Context menu
    nuiRoot.attachChild(m_contextMenu);
    m_contextMenu.setDepth(0.f);

    // Dungeon panel
    nuiRoot.attachChild(m_dungeonPanel);
    m_dungeonPanel.setCentered(true);
    m_dungeonPanel.setSize({4 * 100 + 25, 125 + 25});
    m_dungeonPanel.setLocalPosition({nuiSize.x / 2.f, nuiSize.y - m_dungeonPanel.size().y / 2.f});
    m_dungeonPanel.lerpable()->saveDefaults();

    // Dungeon sidebar
    nuiRoot.attachChild(m_dungeonSidebar);
    m_dungeonSidebar.setCentered(true);
    m_dungeonSidebar.setSize({2 * 100 + 25, 5 * 125 + 25});
    m_dungeonSidebar.setLocalPosition({nuiSize.x - m_dungeonSidebar.size().x / 2.f, nuiSize.y / 2.f});
    m_dungeonSidebar.lerpable()->saveDefaults();
    m_dungeonSidebar.immediateReduce();

    // Dungeon summary
    nuiRoot.attachChild(m_dungeonSummary);
    m_dungeonSummary.useData(m_dungeonData);

    // Dungeon inter
    dungeonRoot.attachChild(m_dungeonInter);
    m_dungeonInter.useData(m_dungeonData);
    m_dungeonInter.setSize({350.f, 450.f});
    m_dungeonInter.setLocalPosition({sceneSize.x / 3.f, sceneSize.y - 450.f - 100.f});

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
    m_dungeonData.save(world::context.selectedWorld().folder);

    // Saving worlds info
    world::context.refreshLastPlayed();
    world::context.save();
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
#if DEBUG_GLOBAL > 0
        // Spawn attacking hero in debug mode
        else if (event.key.code == sf::Keyboard::Equal) {
            m_dungeonGraph.reconstructFromData();
            if (!m_dungeonInter.hasChild(m_dungeonHero)) {
                m_dungeonInter.attachChild(m_dungeonHero);
                m_dungeonHero.useGraph(m_dungeonGraph);
            }
        }
#endif
    }


    return State::handleEvent(event);
}

