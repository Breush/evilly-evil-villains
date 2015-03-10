#pragma once

#include <string>

// Convert string <-> wstring (format used by SFML and pugixml)
inline std::wstring toWString(const std::string& s);
inline std::string toString(const std::wstring& ws);

template<typename T>
std::basic_string<T>& lowercase(std::basic_string<T>& str);

template<typename T>
std::basic_string<T>& filterSpecial(std::basic_string<T>& str);

#include "string.inl"
