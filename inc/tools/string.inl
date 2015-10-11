
#include <algorithm> // transform
#include <cwctype>
#include <cctype>
#include <sstream>

#include "tools/debug.hpp"

//------------------------------//
//----- wstring <-> string -----//

template<typename T>
inline std::string toString(const T& t)
{
    std::stringstream s;
    s << t;
    return s.str();
}

template<>
inline std::string toString(const std::wstring& ws)
{
    const wchar_t* cws = ws.c_str();
    const auto wn = std::wcstombs(nullptr, cws, 0);
    assert(wn != size_t(-1));

    std::vector<char> vs(wn + 1);
    std::wcsrtombs(vs.data(), &cws, wn + 1, nullptr);
    assert(cws == nullptr);

    return std::string(vs.data(), wn);
}

template<typename T>
inline std::wstring toWString(const T& t)
{
    std::wstringstream ws;
    ws << t;
    return ws.str();
}

template<>
inline std::wstring toWString(const std::string& s)
{
    const char* cs = s.c_str();
    const auto wn = std::mbstowcs(nullptr, cs, 0);
    assert(wn != size_t(-1));

    std::vector<wchar_t> vws(wn + 1);
    std::mbsrtowcs(vws.data(), &cs, wn + 1, nullptr);
    assert(cs == nullptr);

    return std::wstring(vws.data(), wn);
}

template<typename T>
inline T to(const std::string& s)
{
    std::stringstream ss(s);
    T t;
    ss >> t;
    return t;
}

template<typename T>
inline T to(const std::wstring& ws)
{
    std::wstringstream ss(ws);
    T t;
    ss >> t;
    return t;
}

//-----------------------//
//----- lower/upper -----//

inline std::string& toLowercase(std::string& str)
{
    std::transform(str.begin(), str.end(), str.begin(), tolower);
    return str;
}

inline std::wstring& toLowercase(std::wstring& str)
{
    std::transform(str.begin(), str.end(), str.begin(), towlower);
    return str;
}

inline std::string& toUppercase(std::string& str)
{
    std::transform(str.begin(), str.end(), str.begin(), toupper);
    return str;
}

inline std::wstring& toUppercase(std::wstring& str)
{
    std::transform(str.begin(), str.end(), str.begin(), towupper);
    return str;
}

//---------------------//
//----- Splitting -----//

inline std::vector<std::wstring> split(const std::wstring& str, const wchar_t delim)
{
    std::wstringstream ss(str);
    std::vector<std::wstring> items;
    std::wstring item;

    while (std::getline(ss, item, delim))
        items.emplace_back(std::move(item));

    return items;
}

//-----------------------------//
//----- Special treatment -----//

inline std::string& filterSpecial(std::string& str, bool lower)
{
    for (auto& c : str)
        if (!isalnum(c))
            c = L'_';

    return (lower)? toLowercase(str) : str;
}

inline std::wstring& filterSpecial(std::wstring& str, bool lower)
{
    for (auto& c : str)
        if (!iswalnum(c))
            c = L'_';

    return (lower)? toLowercase(str) : str;
}

//-----------------------//
//----- SFML addons -----//

template<class Bounded_t>
inline sf::Vector2f boundsSize(const Bounded_t& bounded)
{
    const auto& bounds = bounded.getLocalBounds();
    return {bounds.left + bounds.width, bounds.top + bounds.height};
}
