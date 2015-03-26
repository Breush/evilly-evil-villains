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

    template<typename T> Rect<T>& operator*=(Rect<T>& r, const Vector2<T>& v)
    {
        r.left *= v.x; r.width  *= v.x;
        r.top  *= v.y; r.height *= v.y;
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
