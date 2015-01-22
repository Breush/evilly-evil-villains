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
    typedef Vector2<uint16> Vector2u16;
    typedef Vector2<uint32> Vector2u32;
    typedef Vector2<uint64> Vector2u64;

    template<typename T> Vector2f v2f(const sf::Vector2<T>& v) { return Vector2f(float(v.x), float(v.y)); }
    template<typename T> Vector2i v2i(const sf::Vector2<T>& v) { return Vector2i(int(v.x), int(v.y)); }
};
