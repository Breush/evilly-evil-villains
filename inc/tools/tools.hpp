#pragma once

// Reverse for
#define rfor(variable, container) for (auto variable = container.rbegin(); variable != container.rend(); ++variable)

// Return if inline statement
#define returnif(...) if (__VA_ARGS__) return

// Typedef definitions
#include "tools/int.hpp"
#include <SFML/System/Vector2.hpp>

namespace sf
{
    typedef sf::Vector2<uint16> Vector2u16;
    typedef sf::Vector2<uint32> Vector2u32;
    typedef sf::Vector2<uint64> Vector2u64;
};
