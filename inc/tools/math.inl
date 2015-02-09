template<typename T1, typename T2>
void clamp(T1& x, T1& y, const T2& maxX, const T2& maxY)
{
    if (x < 0) x = 0;
    else if (x >= maxX) x = maxX - 1;

    if (y < 0) y = 0;
    else if (y >= maxY) y = maxY - 1;
}

template<typename T1, typename T2>
void clamp(sf::Vector2<T1>& pos, const T2& maxX, const T2& maxY)
{
    return clamp(pos.x, pos.y, maxX, maxY);
}

template<typename T1, typename T2>
void clamp(T1& x, T1& y, const sf::Vector2<T2>& maxPos)
{
    return clamp(x, y, maxPos.x, maxPos.y);
}

template<typename T1, typename T2>
void clamp(sf::Vector2<T1>& pos, const sf::Vector2<T2>& maxPos)
{
    return clamp(pos.x, pos.y, maxPos.x, maxPos.y);
}

template<typename T>
inline T interpolate(T a, T b, float t)
{
    return T(a + (b - a) * t);
}
