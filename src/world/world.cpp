#include "world/world.hpp"
#include "world/flat.hpp"
#include "world/fractal.hpp"

#include "tools/debug.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

// Static variable
Block* World::m_blocks(nullptr);

World::World(State::Context inContext, Game::Context& inGameContext)
    : m_context(inContext)
    , m_size(WORLD_SIZE_X, WORLD_SIZE_Y)
    , m_gameContext(inGameContext)
{
    // Registering
    m_gameContext.world = this;

    // Allocations
    m_blocks = new Block[WORLD_SIZE_XY];

    // Initializations
    m_viewSize = sf::Vector2<uint32>(context().window->getView().getSize());
    m_viewSize.x = BLOCK_PIXEL_SIZE_X + m_viewSize.x / BLOCK_PIXEL_SIZE_X;
    m_viewSize.y = BLOCK_PIXEL_SIZE_Y + m_viewSize.y / BLOCK_PIXEL_SIZE_Y;
}

World::~World()
{
    delete[] m_blocks;
}

//-------------------//
//----- Routine -----//

void World::init()
{
    // TODO Good init procedure
    //createFlat();
    createFractal();
}

void World::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    sf::Vector2u32 camPos(m_gameContext.camera->position());
    sf::Vector2f pitch(sf::Vector2f(camPos) - m_gameContext.camera->position());

    // Loop on visible blocks -- TODO Stupid print - have a const running image
    sf::RectangleShape sprite;
    sprite.setSize(sf::Vector2f(BLOCK_PIXEL_SIZE_X, BLOCK_PIXEL_SIZE_Y));
    for (uint32 x = 0; x < viewSize().x; ++x)
    for (uint32 y = 0; y < viewSize().y; ++y) {
        sf::Vector2u32 pos((camPos.x + x) % WORLD_SIZE_X, camPos.y + y);
        sprite.setPosition((pitch.x + x) * BLOCK_PIXEL_SIZE_X, (pitch.y + y) * BLOCK_PIXEL_SIZE_Y);
        sprite.setFillColor(m_blocks[coo(pos)].color());
        target.draw(sprite, states);
    }
}

void World::update(sf::Time)
{
}

//----------------//
//----- Init -----//

//------------------//
//----- Create -----//

void World::createFlat()
{
    // TODO Get somehow the parameters for the layers
    std::vector<FlatWorld::Layer> layers;

    layers.push_back({10, Blocks::DIRT});
    layers.push_back({11, Blocks::ROCK});
    layers.push_back({14, Blocks::LAVA});
    layers.push_back({15, Blocks::CRUST});

    FlatWorld::create(m_blocks, layers);
}

void World::createFractal()
{
    FractalWorld fractalWorld(m_blocks);
    fractalWorld.create();
}

//-----------------------//
//----- Environment -----//

bool World::touch(sf::Vector2u32& block, Blocks::ID neighbourg, bool diagonally)
{
    // y == 0
    returnif (m_blocks[coo(subX(block, 1u))].id == neighbourg) true;
    returnif (m_blocks[coo(addX(block, 1u))].id == neighbourg) true;

    // y == -1
    if (subableY(block.y, 1u)) {
        sf::Vector2u32 pos(subY(block, 1u));
        returnif (m_blocks[coo(pos)].id == neighbourg) true;

        if (diagonally) {
            returnif (m_blocks[coo(subX(pos, 1u))].id == neighbourg) true;
            returnif (m_blocks[coo(addX(pos, 1u))].id == neighbourg) true;
        }
    }

    // y == -1
    if (addableY(block.y, 1u)) {
        sf::Vector2u32 pos(addY(block, 1u));
        returnif (m_blocks[coo(pos)].id == neighbourg) true;

        if (diagonally) {
            returnif (m_blocks[coo(subX(pos, 1u))].id == neighbourg) true;
            returnif (m_blocks[coo(addX(pos, 1u))].id == neighbourg) true;
        }
    }

    // Nada
    return false;
}
