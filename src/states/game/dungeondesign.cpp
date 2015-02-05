#include "states/game/dungeondesign.hpp"

#include "core/application.hpp"
#include "resources/musicplayer.hpp"
#include "world/context.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

GameDungeonDesignState::GameDungeonDesignState(StateStack& stack)
    : State(stack)
    , m_uiCore()
{
    auto& resolution = Application::context().resolution;

    // Stop music if any
    Application::context().music.stop();

    // Dungeon data
    massert(!world::context.info->folder.empty(), "Selected world is in an empty folder.");
    m_dungeonData.load("worlds/" + world::context.info->folder + "dungeon.xml");

    // Dungeon inter
    m_uiCore.add(&m_dungeonInter);
    m_dungeonInter.useData(m_dungeonData);
    m_dungeonInter.setSize({350, 450});
    m_dungeonInter.setPosition((resolution - m_dungeonInter.size()) / 2.f);

    // Dungeon panel
    m_uiCore.add(&m_dungeonPanel);
    m_dungeonPanel.setPosition({200, resolution.y - 100});
    m_dungeonPanel.setSize({resolution.x - 400, 100});
}

void GameDungeonDesignState::draw()
{
    Application::context().window.draw(m_uiCore);
}

bool GameDungeonDesignState::update(const sf::Time& dt)
{
    m_uiCore.update(dt);
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

