#pragma once

// Reverse for
#define rfor(variable, container) for (auto variable = container.rbegin(); variable != container.rend(); ++variable)

// Return if inline statement
#define returnif(...) if (__VA_ARGS__) return

// Typedef definitions
#include "tools/int.hpp"

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <map>

namespace sf
{
    typedef Vector2<uint16> Vector2u16;
    typedef Vector2<uint32> Vector2u32;
    typedef Vector2<uint64> Vector2u64;

    template<typename T> Vector2f v2f(const Vector2<T>& v) { return Vector2f(float(v.x), float(v.y)); }
    template<typename T> Vector2i v2i(const Vector2<T>& v) { return Vector2i(int(v.x), int(v.y)); }

    template<typename T> Vector2<T> vdiv(const Vector2<T>& v1, const Vector2<T>& v2) { return Vector2<T>(v1.x / v2.x, v1.y / v2.y); }
    template<typename T> Vector2<T> vmul(const Vector2<T>& v1, const Vector2<T>& v2) { return Vector2<T>(v1.x * v2.x, v1.y * v2.y); }

    template<typename T> Vector2<T> vsub(const Vector2<T>& v, const T& z) { return Vector2<T>(v.x - z, v.y - z); }

    template<typename T> Vector2<T>& vsubin(Vector2<T>& v, const T& x) {
        v.x -= x; v.y -= x;
        return v;
    }

    template<typename T> Rect<T>& raddin(Rect<T>& r, const Vector2<T>& v) {
        r.left += v.x; r.top += v.y;
        return r;
    }

    template<typename T> Rect<T>& rmulin(Rect<T>& r, const Vector2<T>& v) {
        r.left *= v.x; r.width  *= v.x;
        r.top  *= v.y; r.height *= v.y;
        return r;
    }

}

namespace tools
{
    template<typename T1, typename T2>
    inline T2 mapFind(const std::map<T1, T2>& map, T1 const& key)
    {
        const auto& element = map.find(key);
        if (element == map.end()) return T2();
        return element->second;
    }
}
