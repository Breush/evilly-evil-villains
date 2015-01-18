template<typename T1, typename T2>
inline bool isInsideRect(sf::Vector2<T1>& point , const sf::Rect<T2>& rect)
{
    return isInsideRect(point.x, point.y, rect);
}

template<typename T1, typename T2>
inline bool isInsideRect(const T1& x, const T1& y, const sf::Rect<T2>& rect)
{
    if (x < rect.left || x > rect.left + rect.width)
        return false;

    if (y < rect.top || y > rect.top + rect.height)
        return false;

    return true;
}
