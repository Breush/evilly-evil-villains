namespace sf
{
    //-------------------//
    //----- Convert -----//

    template<typename T> Vector2f v2f(const Vector2<T>& v)
    {
        return Vector2f(float(v.x), float(v.y));
    }

    template<typename T> Vector2i v2i(const Vector2<T>& v)
    {
        return Vector2i(int(v.x), int(v.y));
    }

    template<typename T> Vector2u v2u(const Vector2<T>& v)
    {
        return Vector2u(uint(v.x), uint(v.y));
    }

    //-----------------------------------//
    //----- Operators vector-vector -----//

    template<typename T> Vector2<T> operator/(const Vector2<T>& v1, const Vector2<T>& v2)
    {
        return Vector2<T>(v1.x / v2.x, v1.y / v2.y);
    }

    template<typename T> Vector2<T> operator*(const Vector2<T>& v1, const Vector2<T>& v2)
    {
        return Vector2<T>(v1.x * v2.x, v1.y * v2.y);
    }

    template<typename T> Vector2<T>& operator/=(Vector2<T>& v1, const Vector2<T>& v2)
    {
        v1.x /= v2.x;
        v1.y /= v2.y;
        return v1;
    }

    template<typename T> Vector2<T>& operator*=(Vector2<T>& v1, const Vector2<T>& v2)
    {
        v1.x *= v2.x;
        v1.y *= v2.y;
        return v1;
    }

    //-------------------------------------//
    //----- Operators vector-constant -----//

    template<typename T> Vector2<T> operator-(const Vector2<T>& v, const T& z)
    {
        return Vector2<T>(v.x - z, v.y - z);
    }

    template<typename T> Vector2<T> operator/(const T& z, const Vector2<T>& v)
    {
        return Vector2<T>(z / v.x, z / v.y);
    }

    //---------------------------------//
    //----- Operators rect-vector -----//

    template<typename T> Rect<T>& operator+=(Rect<T>& r, const Vector2<T>& v)
    {
        r.left += v.x; r.top += v.y;
        return r;
    }

    template<typename T> Rect<T>& operator-=(Rect<T>& r, const Vector2<T>& v)
    {
        r.left -= v.x; r.top -= v.y;
        return r;
    }

    template<typename T> Rect<T>& operator*=(Rect<T>& r, const Vector2<T>& v)
    {
        r.left *= v.x; r.width  *= v.x;
        r.top  *= v.y; r.height *= v.y;
        return r;
    }

    template<typename T> Rect<T>& operator/=(Rect<T>& r, const Vector2<T>& v)
    {
        r.left /= v.x; r.width  /= v.x;
        r.top  /= v.y; r.height /= v.y;
        return r;
    }

    template<typename T> Rect<T> operator+(const Rect<T>& r, const Vector2<T>& v)
    {
        Rect<T> t(r);
        return t += v;
    }

    template<typename T> Rect<T> operator-(const Rect<T>& r, const Vector2<T>& v)
    {
        Rect<T> t(r);
        return t -= v;
    }

    template<typename T> Rect<T> operator*(const Rect<T>& r, const Vector2<T>& v)
    {
        Rect<T> t(r);
        return t *= v;
    }

    template<typename T> Rect<T> operator/(const Rect<T>& r, const Vector2<T>& v)
    {
        Rect<T> t(r);
        return t /= v;
    }

    //---------------------------//
    //----- Stream operator -----//

    template<typename T> std::ostream& operator<<(std::ostream& os, const Vector2<T>& v)
    {
        return os << '(' << v.x << ',' << v.y << ')';
    }

    template<typename T> std::wostream& operator<<(std::wostream& os, const Vector2<T>& v)
    {
        return os << L'(' << v.x << L',' << v.y << L')';
    }

    template<typename T> std::ostream& operator<<(std::ostream& os, const Rect<T>& r)
    {
        return os << '(' << r.left << ',' << r.top << ':' << r.width << ',' << r.height << ')';
    }

    template<typename T> std::wostream& operator<<(std::wostream& os, const Rect<T>& r)
    {
        return os << L'(' << r.left << L',' << r.top << L':' << r.width << L',' << r.height << L')';
    }
}

namespace tools
{
    template<typename T> sf::Rect<T> intersect(const sf::Rect<T>& r1, const sf::Rect<T>& r2)
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
}

namespace std
{
    template <> struct hash<sf::Vector2u>
    {
        uint32 operator()(const sf::Vector2u& key) const
        {
            return (key.x + key.y) * (key.x + key.y + 1u) / 2u + key.y;
        }
    };
}
