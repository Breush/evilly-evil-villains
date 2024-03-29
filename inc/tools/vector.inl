namespace sf
{
    //-------------------//
    //----- Convert -----//

    template<typename T> inline
    Vector2f v2f(const Vector2<T>& v)
    {
        return Vector2f(static_cast<float>(v.x), static_cast<float>(v.y));
    }

    template<typename T> inline
    Vector2i v2i(const Vector2<T>& v)
    {
        return Vector2i(static_cast<int>(v.x), static_cast<int>(v.y));
    }

    template<typename T> inline
    Vector2u v2u(const Vector2<T>& v)
    {
        return Vector2u(static_cast<uint>(v.x), static_cast<uint>(v.y));
    }

    template<typename T> inline
    Vector2u8 v2u8(const Vector2<T>& v)
    {
        return Vector2u8(static_cast<uint8>(v.x), static_cast<uint8>(v.y));
    }

    template<typename T> inline
    FloatRect toFloatRect(const Rect<T>& r)
    {
        return FloatRect(float(r.left), float(r.top), float(r.width), float(r.height));
    }

    //-----------------------------------//
    //----- Operators vector-vector -----//

    template<typename T> inline
    Vector2<T> operator/(const Vector2<T>& v1, const Vector2<T>& v2)
    {
        return Vector2<T>(v1.x / v2.x, v1.y / v2.y);
    }

    template<typename T> inline
    Vector2<T> operator*(const Vector2<T>& v1, const Vector2<T>& v2)
    {
        return Vector2<T>(v1.x * v2.x, v1.y * v2.y);
    }

    template<typename T> inline
    Vector2<T>& operator/=(Vector2<T>& v1, const Vector2<T>& v2)
    {
        v1.x /= v2.x;
        v1.y /= v2.y;
        return v1;
    }

    template<typename T> inline
    Vector2<T>& operator*=(Vector2<T>& v1, const Vector2<T>& v2)
    {
        v1.x *= v2.x;
        v1.y *= v2.y;
        return v1;
    }

    //-------------------------------------//
    //----- Operators vector-constant -----//

    template<typename T> inline
    Vector2<T> operator-(const Vector2<T>& v, const T& z)
    {
        return Vector2<T>(v.x - z, v.y - z);
    }

    template<typename T> inline
    Vector2<T> operator+(const Vector2<T>& v, const T& z)
    {
        return Vector2<T>(v.x + z, v.y + z);
    }

    template<typename T> inline
    Vector2<T> operator/(const T& z, const Vector2<T>& v)
    {
        return Vector2<T>(z / v.x, z / v.y);
    }

    //---------------------------------//
    //----- Operators rect-vector -----//

    template<typename T> inline
    Rect<T>& operator+=(Rect<T>& r, const Vector2<T>& v)
    {
        r.left += v.x; r.top += v.y;
        return r;
    }

    template<typename T> inline
    Rect<T>& operator-=(Rect<T>& r, const Vector2<T>& v)
    {
        r.left -= v.x; r.top -= v.y;
        return r;
    }

    template<typename T> inline
    Rect<T>& operator*=(Rect<T>& r, const Vector2<T>& v)
    {
        r.left *= v.x; r.width  *= v.x;
        r.top  *= v.y; r.height *= v.y;
        return r;
    }

    template<typename T> inline
    Rect<T>& operator/=(Rect<T>& r, const Vector2<T>& v)
    {
        r.left /= v.x; r.width  /= v.x;
        r.top  /= v.y; r.height /= v.y;
        return r;
    }

    template<typename T> inline
    Rect<T> operator+(const Rect<T>& r, const Vector2<T>& v)
    {
        Rect<T> t(r);
        return t += v;
    }

    template<typename T> inline
    Rect<T> operator-(const Rect<T>& r, const Vector2<T>& v)
    {
        Rect<T> t(r);
        return t -= v;
    }

    template<typename T> inline
    Rect<T> operator*(const Rect<T>& r, const Vector2<T>& v)
    {
        Rect<T> t(r);
        return t *= v;
    }

    template<typename T> inline
    Rect<T> operator/(const Rect<T>& r, const Vector2<T>& v)
    {
        Rect<T> t(r);
        return t /= v;
    }

    //---------------------------//
    //----- Stream operator -----//

    template<typename T> inline
    std::ostream& operator<<(std::ostream& os, const Vector2<T>& v)
    {
        return os << '(' << v.x << ',' << v.y << ')';
    }

    template<typename T> inline
    std::wostream& operator<<(std::wostream& os, const Vector2<T>& v)
    {
        return os << L'(' << v.x << L',' << v.y << L')';
    }

    template<typename T> inline
    std::ostream& operator<<(std::ostream& os, const Rect<T>& r)
    {
        return os << '(' << r.left << ',' << r.top << ':' << r.width << ',' << r.height << ')';
    }

    template<typename T> inline
    std::wostream& operator<<(std::wostream& os, const Rect<T>& r)
    {
        return os << L'(' << r.left << L',' << r.top << L':' << r.width << L',' << r.height << L')';
    }

    // Special cases with unsigned char

    template<> inline
    std::ostream& operator<<(std::ostream& os, const Vector2<uint8>& v)
    {
        return os << '(' << static_cast<uint>(v.x) << ',' << static_cast<uint>(v.y) << ')';
    }

    template<> inline
    std::wostream& operator<<(std::wostream& os, const Vector2<uint8>& v)
    {
        return os << L'(' << static_cast<uint>(v.x) << L',' << static_cast<uint>(v.y) << L')';
    }
}

namespace tools
{
    template<typename T> inline
    sf::Rect<T> intersect(const sf::Rect<T>& r1, const sf::Rect<T>& r2)
    {
        sf::Rect<T> r;

        if (r1.width < 0.f || r1.height < 0.f) return r2;
        if (r2.width < 0.f || r2.height < 0.f) return r1;

        r.left = std::max(r1.left, r2.left);
        r.top = std::max(r1.top, r2.top);
        r.width = std::min(r1.left + r1.width, r2.left + r2.width) - r.left;
        r.height = std::min(r1.top + r1.height, r2.top + r2.height) - r.top;

        return r;
    }

    template<typename T> inline
    bool areIntersecting(const sf::Rect<T>& r1, const sf::Rect<T>& r2)
    {
        if (r1.width < 0.f || r1.height < 0.f) return false;
        if (r2.width < 0.f || r2.height < 0.f) return false;

        return (r1.left < r2.left + r2.width && r1.left + r1.width > r2.left
                && r1.top > r2.top + r2.height && r1.top + r1.height < r2.top);
    }
}

namespace std
{
    template <> struct hash<sf::Vector2<uint8>>
    {
        uint8 operator()(const sf::Vector2<uint8>& key) const
        {
            return (key.x + key.y) * (key.x + key.y + 1u) / 2u + key.y;
        }
    };

    template <> struct hash<sf::Vector2u>
    {
        uint32 operator()(const sf::Vector2u& key) const
        {
            return (key.x + key.y) * (key.x + key.y + 1u) / 2u + key.y;
        }
    };
}
