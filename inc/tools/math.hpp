#pragma once

#include <SFML/Graphics/Rect.hpp>

//-----------------------//
//----- Inside Rect -----//

// Returns true if point is inside rect

template<typename T1, typename T2>
bool isInsideRect(const sf::Vector2<T1>& point , const sf::Rect<T2>& rect);

template<typename T1, typename T2>
bool isInsideRect(const T1& x, const T1& y, const sf::Rect<T2>& rect);

// Clamping -> set coordonates between 0 and max - 1

template<typename T1, typename T2>
void clamp(T1& x, T1& y, const T2& maxX, const T2& maxY);

template<typename T1, typename T2>
void clamp(sf::Vector2<T1>& pos, const T2& maxX, const T2& maxY);

template<typename T1, typename T2>
void clamp(T1& x, T1& y, const sf::Vector2<T2>& maxPos);

template<typename T1, typename T2>
void clamp(sf::Vector2<T1>& pos, const sf::Vector2<T2>& maxPos);

#include "math.inl"
