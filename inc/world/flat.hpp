#pragma once

#include "world/identifiers.hpp"

#include "tools/int.hpp"

#include <vector>

class Block;

namespace FlatWorld
{
    struct Layer
    {
        uint32 y;
        Blocks::ID id;
    };

    void fillLine(Block* line, Blocks::ID id);
    void create(Block*& blocks, const std::vector<Layer>& layers);
};
