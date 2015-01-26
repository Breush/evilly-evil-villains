#include "world/flat.hpp"

#include "world/world.hpp"
#include "world/block.hpp"

#include "tools/debug.hpp"

#include <memory>

void FlatWorld::fillLine(Block* line, Blocks::ID id)
{
    // Note: may be optimized by doing size-increasing memcpy()
    for (uint32 x = 0; x < WORLD_SIZE_X; ++x)
        line[x].id = id;
}

void FlatWorld::create(Block*& blocks, const std::vector<FlatWorld::Layer>& layers)
{
    Block line[WORLD_SIZE_X];

    // If not precised, fill with air before
    fillLine(line, Blocks::AIR);

    // For each layer
    uint32 y = 0;
    for (auto& layer : layers) {
        massert(layer.y < WORLD_SIZE_Y, "Wrong layer height: out of range");

        // Fill with block
        // Note: this trick works because lines are stored continually in memory
        while (y < layer.y) {
            uint32 start = (y << WORLD_SIZE_LOG_X);
            std::copy(line, line + WORLD_SIZE_X, blocks + start);
            ++y;
        }

        // Set next layer
        fillLine(line, layer.id);
    }

    // Finishing filling
    while (y < WORLD_SIZE_Y) {
        uint32 start = (y << WORLD_SIZE_LOG_X);
        std::copy(line, line + WORLD_SIZE_X, blocks + start);
        ++y;
    }
}
