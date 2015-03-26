#pragma once

#include "tools/int.hpp"

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>

namespace sf
{
    // Typedef
    using Vector2u16 = Vector2<uint16>;
    using Vector2u32 = Vector2<uint32>;
    using Vector2u64 = Vector2<uint64>;

    // Convert
    template<typename T> Vector2f v2f(const Vector2<T>& v);
    template<typename T> Vector2i v2i(const Vector2<T>& v);
    template<typename T> Vector2u v2u(const Vector2<T>& v);

    // Operators vector-vector
    template<typename T> Vector2<T> operator/(const Vector2<T>& v1, const Vector2<T>& v2);
    template<typename T> Vector2<T> operator*(const Vector2<T>& v1, const Vector2<T>& v2);

    // Operators vector-constant
    template<typename T> Vector2<T> operator-(const Vector2<T>& v, const T& z);
    template<typename T> Vector2<T> operator/(const T& z, const Vector2<T>& v);

    // Operators rect-vector
    template<typename T> Rect<T>& operator+=(Rect<T>& r, const Vector2<T>& v);
    template<typename T> Rect<T>& operator*=(Rect<T>& r, const Vector2<T>& v);
}

namespace std
{
    // Using Cantor hash for vectors
    template <> struct hash<sf::Vector2u>;
}

#include "tools/vector.inl"
