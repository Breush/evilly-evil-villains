#pragma once

#include <string>
#include <SFML/Graphics/Text.hpp>

// Convert string <-> wstring (format used by SFML and pugixml)
template<typename T> inline std::string toString(const T& t);
template<> inline std::string toString(const std::wstring& ws);
template<typename T> inline std::wstring toWString(const T& t);
template<> inline std::wstring toWString(const std::string& s);
template<typename T> inline T to(const std::string& s);
template<typename T> inline T to(const std::wstring& ws);

// To lowercase/uppercase
inline std::string& toLowercase(std::string& str);
inline std::wstring& toLowercase(std::wstring& str);
inline std::string& toUppercase(std::string& str);
inline std::wstring& toUppercase(std::wstring& str);

// Manipulations
inline std::vector<std::wstring> split(const std::wstring& str, const wchar_t delim = L' ');
template<typename String_t> inline String_t join(const std::vector<String_t>& v, const String_t& glue = L" ");
template<typename String_t> inline String_t prefix(const String_t& s1, const String_t& s2);

// Replace all non-alphanum character by an underscore
// and make the string to lowercase if lower is set to true
inline std::string& filterSpecial(std::string& str, bool lower = true);
inline std::wstring& filterSpecial(std::wstring& str, bool lower = true);

// Return the local bounds of a element (i.e. a text)
template <class Bounded_t> inline sf::Vector2f boundsSize(const Bounded_t& bounded);

#include "string.inl"
