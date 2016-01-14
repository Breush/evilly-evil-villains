template<typename T>
inline T gcd(T a, T b)
{
    return (b == 0)? a : gcd(b, a % b);
}

template<typename T>
inline std::wstring aspectRatio(T w, T h)
{
    auto g = gcd(w, h);
    std::wstringstream s;

    auto a = w/g;
    auto b = h/g;

    // We want 16:10 ratio to appear as such and not 8:5
    if (a == 8 && (g % 2 == 0)) {
        a *= 2;
        b *= 2;
    }

    // 85:48 ratio as 1360x768 is officially an almost 16:9
    // 683:384 ratio as 1366x768 also
    else if ((a == 85 && b == 48)
             || (a == 683 && b == 384)) {
        s << L'≈';
        a = 16;
        b = 9;
    }

    // 25:16 ratio as 1600x1024 is officially an almost 16:10
    else if (a == 25 && b == 16) {
        s << L'≈';
        a = 16;
        b = 10;
    }

    s << a << L':' << b;
    return s.str();
}

template<typename T1, typename T2>
inline void clamp(T1& x, T1& y, const T2& maxX, const T2& maxY)
{
    if (x < 0) x = 0;
    else if (x >= maxX) x = maxX - 1;

    if (y < 0) y = 0;
    else if (y >= maxY) y = maxY - 1;
}

template<typename T1, typename T2>
inline void clamp(sf::Vector2<T1>& pos, const T2& maxX, const T2& maxY)
{
    return clamp(pos.x, pos.y, maxX, maxY);
}

template<typename T1, typename T2>
inline void clamp(T1& x, T1& y, const sf::Vector2<T2>& maxPos)
{
    return clamp(x, y, maxPos.x, maxPos.y);
}

template<typename T1, typename T2>
inline void clamp(sf::Vector2<T1>& pos, const sf::Vector2<T2>& maxPos)
{
    return clamp(pos.x, pos.y, maxPos.x, maxPos.y);
}

template<typename T>
inline T interpolate(T a, T b, float t)
{
    return T(a + (b - a) * t);
}
