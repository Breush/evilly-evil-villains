#include "states/game/dungeondesign.hpp"

#include "core/application.hpp"
#include "resources/identifiers.hpp"
#include "world/context.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

GameDungeonDesignState::GameDungeonDesignState(StateStack& stack)
    : State(stack)
    , m_dungeonInter(m_contextMenu)
    , m_dungeonPanel(m_dungeonSidebar)
{
    // During game, disable key repeat
    Application::context().window.setKeyRepeatEnabled(false);

    auto& resolution = Application::context().resolution;

    // Stop music if any
    Application::context().musics.stop();

    // Dungeon data
    massert(!world::context.info->folder.empty(), "Selected world is in an empty folder.");
    m_dungeonData.load("worlds/" + world::context.info->folder + "dungeon.xml");

    // Context menu
    sceneLayer(LayerID::NUI).attachChild(m_contextMenu);
    m_contextMenu.setDepth(0.f);

    // Dungeon inter
    sceneLayer(LayerID::DUNGEON_DESIGN).attachChild(m_dungeonInter);
    m_dungeonInter.setDepth(50.f);
    m_dungeonInter.useData(m_dungeonData);
    m_dungeonInter.setSize({350, 450});
    m_dungeonInter.setCentered(true);
    m_dungeonInter.setLocalPosition(resolution / 2.f);

    // Dungeon panel
    sceneLayer(LayerID::NUI).attachChild(m_dungeonPanel);
    m_dungeonPanel.setCentered(true);
    m_dungeonPanel.setSize({4 * 100 + 25, 125 + 25});
    m_dungeonPanel.setLocalPosition({resolution.x / 2.f, resolution.y - m_dungeonPanel.size().y / 2.f});
    m_dungeonPanel.lerpable()->saveDefaults();

    // Dungeon sidebar
    sceneLayer(LayerID::NUI).attachChild(m_dungeonSidebar);
    m_dungeonSidebar.setCentered(true);
    m_dungeonSidebar.setSize({2 * 100 + 25, 5 * 125 + 25});
    m_dungeonSidebar.setLocalPosition({resolution.x - m_dungeonSidebar.size().x / 2.f, resolution.y / 2.f});
    m_dungeonSidebar.lerpable()->saveDefaults();
    m_dungeonSidebar.immediateReduce();

    // Decorum
    sceneLayer(LayerID::DUNGEON_DESIGN).attachChild(m_decorumBack);
    m_decorumBack.setDepth(100.f);
    m_decorumBack.setTexture(TextureID::DUNGEON_SCENE_GRASSYHILLS_BACK);

    sceneLayer(LayerID::DUNGEON_DESIGN).attachChild(m_decorumFront);
    m_decorumFront.setDepth(10.f);
    m_decorumFront.setTexture(TextureID::DUNGEON_SCENE_GRASSYHILLS_FRONT);
}

void GameDungeonDesignState::onQuit() noexcept
{
    // TODO Remove suffix (used to not compromise svn archive)
    m_dungeonData.save("worlds/" + world::context.info->folder + "dungeon_saved.xml");
    world::context.updateLastPlayed();
}

bool GameDungeonDesignState::handleEvent(const sf::Event& event)
{
    // Global events
    m_dungeonInter.handleGlobalEvent(event);
    m_contextMenu.handleGlobalEvent(event);

    // Escape pressed, trigger the pause screen
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape) {
            stackPush(StateID::GAME_PAUSE);
        }
    }

    return State::handleEvent(event);
}

