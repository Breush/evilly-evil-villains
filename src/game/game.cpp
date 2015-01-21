#include "core/application.hpp"
#include "game/game.hpp"
#include "player/player.hpp"
#include "tools/tools.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

//-------------------//
//----- Context -----//

Game::Context::Context(Camera& inCamera)
    : camera(&inCamera)
{
}

//----------------//
//----- Game -----//

Game::Game()
    : m_viewSize(Application::context().resolution)
    , m_camera(m_viewSize)
    , m_gameContext(m_camera)
{
}

Game::~Game()
{
}

//-------------------//
//----- Routine -----//

void Game::init()
{
}

void Game::update(sf::Time)
{
}

bool Game::handleEvent(const sf::Event& event)
{
    returnif (event.type != sf::Event::KeyPressed) true;

    auto& code = event.key.code;

    // FIXME Arrow movement
    if (code == sf::Keyboard::Up)
        m_camera.move( 0.0f,-0.5f);
    else if (code == sf::Keyboard::Down)
        m_camera.move( 0.0f, 0.5f);
    else if (code == sf::Keyboard::Left)
        m_camera.move(-0.5f, 0.0f);
    else if (code == sf::Keyboard::Right)
        m_camera.move( 0.5f, 0.0f);

    // FIXME Centering camera
    if (code == sf::Keyboard::P)
        m_camera.center(gameContext().player->position());

    return true;
}
