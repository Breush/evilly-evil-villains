#pragma once

#define _USE_MATH_DEFINES // Useful on Windows
#include <cmath>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>

//--------------------//
//----- Clamping -----//

// Clamping -> set coordonates between 0 and max - 1

template<typename T1, typename T2>
void clamp(T1& x, T1& y, const T2& maxX, const T2& maxY);

template<typename T1, typename T2>
void clamp(sf::Vector2<T1>& pos, const T2& maxX, const T2& maxY);

template<typename T1, typename T2>
void clamp(T1& x, T1& y, const sf::Vector2<T2>& maxPos);

template<typename T1, typename T2>
void clamp(sf::Vector2<T1>& pos, const sf::Vector2<T2>& maxPos);

//-------------------------//
//----- Interpolation -----//

// 0.f <= t <= 1.f

template<typename T>
inline T interpolate(T a, T b, float t);

#include "math.inl"
