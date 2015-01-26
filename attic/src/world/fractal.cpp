#include "world/fractal.hpp"

#include "world/world.hpp"
#include "world/block.hpp"

#include "tools/debug.hpp"
#include "tools/tools.hpp"
#include "tools/random.hpp"

#include <memory>

FractalWorld::FractalWorld(Block*& blocks)
    : m_seed(1)
    , m_fractions(1u << (WORLD_SIZE_LOG_X - 5u))
    , m_ySalt(0.5)
    , m_ySaltDec(1.1)
    , m_dirtDepth(2)
    , m_blocks(blocks)
    , m_rfFraction(0x30ae180au)
{
}

//-----------------------//
//----- Interactive -----//

void FractalWorld::create()
{
    // Fill world with air
    erase();

    // Doing each fraction for max height
    sf::Vector2u32 start, stop;
    stop.y = Random::randu32(m_seed, m_rfFraction, fractions(), 25, 60);
    for (uint16 fraction = 0; fraction < fractions(); ++fraction) {
        start.x = fraction * WORLD_SIZE_X_MONE / fractions();
        start.y = stop.y;
        stop.x = (fraction + 1) * WORLD_SIZE_X_MONE / fractions();
        stop.y = Random::randu32(seed(), m_rfFraction, fraction + 1, 25, 60);
        heightMaker(start, stop, ySalt());
    }

    // Fill rock and dirt
    rockarize();
}

void FractalWorld::erase()
{
    // Note: may be optimized by doing size-increasing memcpy()
    for (uint32 p = 0; p < WORLD_SIZE_XY; ++p)
        m_blocks[p].id = Blocks::AIR;
}

//--------------------//
//----- Creation -----//

void FractalWorld::heightMaker(const sf::Vector2u32& start, const sf::Vector2u32& stop, double salt)
{
    sf::Vector2u32 middle = (start + stop) / 2u;

    // Stop recursion when middle and start are the same
    if (middle.x == start.x) {
        m_blocks[World::coo(start)].id = Blocks::ROCK;
        m_blocks[World::coo(stop)].id = Blocks::ROCK;
    }
    // Continue on smaller parts
    else {
        // Adding salt
        middle.y += Random::randd(seed(), m_rfFraction, start.x, -salt, salt) * abs(stop.y - start.y);
        m_blocks[World::coo(middle)].id = Blocks::ROCK;

        // Continue on
        heightMaker(start, sf::Vector2u32(middle.x-1, middle.y), salt/m_ySaltDec);
        heightMaker(sf::Vector2u32(middle.x+1, middle.y), stop, salt/m_ySaltDec);
    }
}

void FractalWorld::rockarize()
{
    sf::Vector2u32 pos;
    uint32 depth;

    // Filling with rocks
    for (pos.x = 0; pos.x < WORLD_SIZE_X; ++pos.x) {
        // First, find rock
        for (pos.y = 0; m_blocks[World::coo(pos)].id == Blocks::AIR; ++pos.y);

        // Fill below
        for (; pos.y < WORLD_SIZE_Y; ++pos.y)
            m_blocks[World::coo(pos)].id = Blocks::ROCK;
    }

    // Add dirt
    returnif (dirtDepth() == 0);
    for (pos.x = 0; pos.x < WORLD_SIZE_X; ++pos.x) {
        // First, find rock
        for (pos.y = 0; m_blocks[World::coo(pos)].id == Blocks::AIR; ++pos.y);

        // Is dirt if can touch air
        for (; World::touch(pos, Blocks::AIR, true) && pos.y < WORLD_SIZE_Y; ++pos.y)
            m_blocks[World::coo(pos)].id = Blocks::DIRT;

        // Fill dirt given depth
        for (depth = 1; depth < dirtDepth() && pos.y < WORLD_SIZE_Y; ++depth, ++pos.y)
            m_blocks[World::coo(pos)].id = Blocks::DIRT;
    }
}
