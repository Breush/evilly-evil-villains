#include "world/block.hpp"

#include "tools/tools.hpp"

sf::Color Block::color()
{
    // TODO Make a register of all blocks, initialized at launch
    returnif (id == Blocks::AIR) sf::Color::Transparent;
    returnif (id == Blocks::DIRT) sf::Color(150, 150, 50);
    returnif (id == Blocks::ROCK) sf::Color(150, 150, 150);
    returnif (id == Blocks::LAVA) sf::Color::Red;
    returnif (id == Blocks::CRUST) sf::Color::Yellow;

    return sf::Color::Blue;
}
