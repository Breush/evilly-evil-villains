#pragma once

#include <SFML/Graphics/Rect.hpp>

//-----------------------//
//----- Inside Rect -----//

// Returns true if point is inside rect

template<typename T1, typename T2>
bool isInsideRect(const sf::Vector2<T1>& point , const sf::Rect<T2>& rect);

template<typename T1, typename T2>
bool isInsideRect(const T1& x, const T1& y, const sf::Rect<T2>& rect);

#include "math.inl"
