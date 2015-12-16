#pragma once

#include "tools/int.hpp"

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>

#include <ostream>

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

    template<typename T> FloatRect toFloatRect(const Rect<T>& r);

    // Operators vector-vector
    template<typename T> Vector2<T> operator/(const Vector2<T>& v1, const Vector2<T>& v2);
    template<typename T> Vector2<T> operator*(const Vector2<T>& v1, const Vector2<T>& v2);
    template<typename T> Vector2<T>& operator/=(Vector2<T>& v1, const Vector2<T>& v2);
    template<typename T> Vector2<T>& operator*=(Vector2<T>& v1, const Vector2<T>& v2);

    // Operators vector-constant
    template<typename T> Vector2<T> operator-(const Vector2<T>& v, const T& z);
    template<typename T> Vector2<T> operator+(const Vector2<T>& v, const T& z);
    template<typename T> Vector2<T> operator/(const T& z, const Vector2<T>& v);

    // Operators rect-vector
    template<typename T> Rect<T>& operator+=(Rect<T>& r, const Vector2<T>& v);
    template<typename T> Rect<T>& operator-=(Rect<T>& r, const Vector2<T>& v);
    template<typename T> Rect<T>& operator*=(Rect<T>& r, const Vector2<T>& v);
    template<typename T> Rect<T>& operator/=(Rect<T>& r, const Vector2<T>& v);
    template<typename T> Rect<T> operator+(const Rect<T>& r, const Vector2<T>& v);
    template<typename T> Rect<T> operator-(const Rect<T>& r, const Vector2<T>& v);
    template<typename T> Rect<T> operator*(const Rect<T>& r, const Vector2<T>& v);
    template<typename T> Rect<T> operator/(const Rect<T>& r, const Vector2<T>& v);

    // Stream operator
    template<typename T> std::ostream& operator<<(std::ostream& os, const Vector2<T>& v);
    template<typename T> std::wostream& operator<<(std::wostream& os, const Vector2<T>& v);
    template<typename T> std::ostream& operator<<(std::ostream& os, const Rect<T>& v);
    template<typename T> std::wostream& operator<<(std::wostream& os, const Rect<T>& v);
}

namespace tools
{
    //! Intersects two rect into one.
    template<typename T> sf::Rect<T> intersect(const sf::Rect<T>& r1, const sf::Rect<T>& r2);
}

namespace std
{
    // Using Cantor hash for vectors
    template <> struct hash<sf::Vector2u>;
}

#include "tools/vector.inl"
