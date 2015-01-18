#include "states/game/game.hpp"

#include "resources/musicplayer.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

GameGameState::GameGameState(StateStack& stack, Context context)
    : State(stack, context)
    , m_game(context)
    , m_world(context, m_game.gameContext())
    , m_player(context, m_game.gameContext())
{
    // Interaction
    context.window->setKeyRepeatEnabled(true);

    // Init world
    m_game.init();
    m_world.init();
    m_player.init();

    // Stop music if any
    context.music->stop();
}

void GameGameState::draw()
{
    sf::RenderWindow& window = *getContext().window;

    window.draw(m_world);
    window.draw(m_player);
}

bool GameGameState::update(sf::Time dt)
{
	m_world.update(dt);
	m_player.update(dt);

    return true;
}

bool GameGameState::handleEvent(const sf::Event& event)
{
    // Escape pressed, trigger the pause screen
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape) {
            stackPush(States::GAME_PAUSE);
        }
        else if (event.key.code == sf::Keyboard::D)
        {
            // FIXME Start donjon
            stackPopPush(States::GAME_DONJON_DESIGN);
        }
    }

    return m_game.handleEvent(event);
}
