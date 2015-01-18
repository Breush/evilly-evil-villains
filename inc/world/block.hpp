#pragma once

#include "identifiers.hpp"

#include <SFML/Graphics/Color.hpp>

class Block
{
public:
    Block() {}
    virtual ~Block() {}

    // Temporary id for testing
    Blocks::ID id;

    // Temporary no-texture for testing
    sf::Color color();
private:
};
