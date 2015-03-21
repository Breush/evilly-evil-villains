#pragma once

#include <string>

// Convert string <-> wstring (format used by SFML and pugixml)
inline std::string toString(const std::wstring& ws);
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

#include "string.inl"
