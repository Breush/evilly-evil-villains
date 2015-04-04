#pragma once

#include <string>
#include <SFML/Graphics/Text.hpp>

// Convert string <-> wstring (format used by SFML and pugixml)
template<typename T> inline std::string toString(const T& t);
template<> inline std::string toString(const std::wstring& ws);
template<typename T> inline std::wstring toWString(const T& t);
template<> inline std::wstring toWString(const std::string& s);

// To lowercase/uppercase
inline std::string& toLowercase(std::string& str);
inline std::wstring& toLowercase(std::wstring& str);
inline std::string& toUppercase(std::string& str);
inline std::wstring& toUppercase(std::wstring& str);

// Replace all non-alphanum character by an underscore
// and make the string to lowercase if lower is set to true
inline std::string& filterSpecial(std::string& str, bool lower = true);
inline std::wstring& filterSpecial(std::wstring& str, bool lower = true);

// Return the local bounds of the text
inline sf::Vector2f boundsSize(const sf::Text& text);

#include "string.inl"
