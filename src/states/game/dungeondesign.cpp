#include "core/application.hpp"
#include "states/game/dungeondesign.hpp"
#include "resources/musicplayer.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

GameDungeonDesignState::GameDungeonDesignState(StateStack& stack)
    : State(stack)
    , m_uiCore()
{
    auto& resolution = Application::context().resolution;

    // Stop music if any
    Application::context().music.stop();

    // Dungeon data
    // TODO Get from game context?
    m_dungeonData.load("worlds/example/dungeon.xml");

    // Dungeon inter
    m_uiCore.add(&m_dungeonInter);
    m_dungeonInter.useData(m_dungeonData);
    m_dungeonInter.setSize({350, 450});
    m_dungeonInter.setPosition((resolution - m_dungeonInter.size()) / 2.f);
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
    // TODO Where to get file name?
    m_dungeonData.save("worlds/example/dungeon_saved.xml");
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

