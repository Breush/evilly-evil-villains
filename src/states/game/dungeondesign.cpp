#include "states/game/dungeondesign.hpp"

#include "core/application.hpp"
#include "resources/musicplayer.hpp"
#include "world/context.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

GameDungeonDesignState::GameDungeonDesignState(StateStack& stack)
    : State(stack)
    , m_uiCore(&m_mouseDetector)
{
    auto& resolution = Application::context().resolution;

    // Hovering system
    m_mouseDetector.init();

    // Stop music if any
    Application::context().music.stop();

    // Dungeon data
    massert(!world::context.info->folder.empty(), "Selected world is in an empty folder.");
    m_dungeonData.load("worlds/" + world::context.info->folder + "dungeon.xml");

    // Dungeon inter
    m_uiCore.add(&m_dungeonInter);
    m_dungeonInter.useData(m_dungeonData);
    m_dungeonInter.setSize({350, 450});
    m_dungeonInter.setLocalPosition((resolution - m_dungeonInter.size()) / 2.f);

    // Dungeon panel
    m_uiCore.add(&m_dungeonPanel);
    m_dungeonPanel.setLocalPosition({200, resolution.y - 100});
    m_dungeonPanel.setSize({resolution.x - 400, 100});

    // Decorum
    m_decorumBack.setTexture(Application::context().textures.get(Textures::DUNGEON_SCENE_GRASSYHILLS_BACK));
    m_decorumFront.setTexture(Application::context().textures.get(Textures::DUNGEON_SCENE_GRASSYHILLS_FRONT));
    tools::adaptTextureRect(m_decorumBack, resolution, tools::Align::END);
    tools::adaptTextureRect(m_decorumFront, resolution, tools::Align::END);

    // Reload everything once so that all is in correct place
    m_uiCore.refresh();
}

void GameDungeonDesignState::draw()
{
    auto& window = Application::context().window;

    // Decorum
    window.draw(m_decorumBack);
    window.draw(m_decorumFront);

    // UI
    window.draw(m_uiCore);

    // Hovering system
    m_mouseDetector.draw();
}

bool GameDungeonDesignState::update(const sf::Time& dt)
{
    m_uiCore.update(dt);
    m_mouseDetector.update(dt);

    return true;
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

    // Let ui core handle events
    m_uiCore.handleEvent(event);
    return false;
}

