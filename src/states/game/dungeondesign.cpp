#include "states/game/dungeondesign.hpp"

#include "core/application.hpp"
#include "resources/musicplayer.hpp"
#include "world/context.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

GameDungeonDesignState::GameDungeonDesignState(StateStack& stack)
    : State(stack)
{
    auto& resolution = Application::context().resolution;

    // Stop music if any
    Application::context().musics.stop();

    // Dungeon data
    massert(!world::context.info->folder.empty(), "Selected world is in an empty folder.");
    m_dungeonData.load("worlds/" + world::context.info->folder + "dungeon.xml");

    // Dungeon inter TODO Remove from NUI
    sceneLayer(Layers::DUNGEON_DESIGN).attachChild(m_dungeonInter);
    m_dungeonInter.useData(m_dungeonData);
    m_dungeonInter.setSize({350, 450});
    m_dungeonInter.setCentered(true);
    m_dungeonInter.setLocalPosition(resolution / 2.f);

    // Dungeon panel
    m_dungeonPanel.setDepth(40);
    sceneLayer(Layers::NUI).attachChild(m_dungeonPanel);
    m_dungeonPanel.setCentered(true);
    m_dungeonPanel.setSize({(4 + 1) * 100, 150});
    m_dungeonPanel.setLocalPosition({resolution.x / 2.f, resolution.y - m_dungeonPanel.size().y / 2.f});
    m_dungeonPanel.lerpable()->saveDefaults();

    // Decorum
    m_decorumBack.setTexture(Application::context().textures.get(Textures::DUNGEON_SCENE_GRASSYHILLS_BACK));
    m_decorumFront.setTexture(Application::context().textures.get(Textures::DUNGEON_SCENE_GRASSYHILLS_FRONT));
}

void GameDungeonDesignState::draw()
{
    auto& window = Application::context().window;

    // Decorum
    window.setView(Application::context().views.get(Views::DUNGEON_DESIGN));
    window.draw(m_decorumBack);

    // UI - Should be removed
    window.setView(Application::context().views.get(Views::DEFAULT));
    State::draw();

    // Decorum
    window.setView(Application::context().views.get(Views::DUNGEON_DESIGN));
    window.draw(m_decorumFront);
}

void GameDungeonDesignState::onQuit()
{
    // TODO Remove suffix (used to not compromise svn archive)
    m_dungeonData.save("worlds/" + world::context.info->folder + "dungeon_saved.xml");
    world::context.updateLastPlayed();
}

bool GameDungeonDesignState::handleEvent(const sf::Event& event)
{
    // Global events
    m_dungeonInter.handleGlobalEvent(event);

    // Escape pressed, trigger the pause screen
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape) {
            stackPush(States::GAME_PAUSE);
        }
    }

    return State::handleEvent(event);
}

