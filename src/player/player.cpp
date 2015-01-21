#include "player/player.hpp"
#include "world/world.hpp"
#include "world/def.hpp"
#include "tools/debug.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

Player::Player(Game::Context& inGameContext)
    : m_onGround(false)
    , m_gameContext(inGameContext)
{
    // Registering
    m_gameContext.player = this;

    // TODO Take mass in account
    setAcceleration(sf::Vector2f(0, 9.81e-1));
}

Player::~Player()
{
}

//-------------------//
//----- Routine -----//

void Player::init()
{
}

void Player::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    // TODO
    sf::Vector2f& camPos(m_gameContext.camera->position());
    sf::Vector2f pos(position().x - camPos.x, position().y - camPos.y);

    if (pos.x < 0.f)
        pos.x += WORLD_SIZE_X;

    // TODO Have a sprite handler (And a PLAYER_SIZE?)
    sf::RectangleShape sprite;
    sprite.setSize(sf::Vector2f(BLOCK_PIXEL_SIZE_X, BLOCK_PIXEL_SIZE_Y));
    sprite.setPosition(pos.x * BLOCK_PIXEL_SIZE_X, pos.y * BLOCK_PIXEL_SIZE_Y);
    sprite.setFillColor(sf::Color::White);
    target.draw(sprite, states);
}

void Player::update(sf::Time dt)
{
    if (!onGround())
    {
        float t = dt.asSeconds();

        m_speed += acceleration() * t;
        m_position += acceleration() * t * t / 2.f + speed() * t;

        // FIXME Debug thing
        if (position().y > 45.f)
            position().y = 0.f;
    }

    checkOnGround();
}

//---------------------//
//----- Positions -----//

void Player::checkOnGround()
{
    sf::Vector2u32 pos(position());

    pos.y += 1; // Player is one block height, TODO, handle collision in a better way

    setOnGround(World::block(pos).id != Blocks::AIR);
}
