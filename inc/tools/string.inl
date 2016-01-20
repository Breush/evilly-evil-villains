
#include <algorithm> // transform
#include <cwctype>
#include <cctype>
#include <sstream>

#include "tools/debug.hpp"
#include "tools/int.hpp"

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

template<>
inline uint8 to(const std::wstring& ws)
{
    std::wstringstream ss(ws);
    uint t;
    ss >> t;
    return static_cast<uint8>(t);
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

//-------------------------//
//----- Manipulations -----//

inline std::vector<std::string> split(const std::string& str, const char delim)
{
    std::stringstream ss(str);
    std::vector<std::string> items;
    std::string item;

    while (std::getline(ss, item, delim))
        if (!item.empty())
            items.emplace_back(std::move(item));

    return items;
}

inline std::vector<std::wstring> split(const std::wstring& str, const wchar_t delim)
{
    std::wstringstream ss(str);
    std::vector<std::wstring> items;
    std::wstring item;

    while (std::getline(ss, item, delim))
        if (!item.empty())
            items.emplace_back(std::move(item));

    return items;
}

template<class String_t>
inline String_t join(const std::vector<String_t>& v, const String_t& glue)
{
    if (v.empty()) return String_t();

    String_t s(v.front());
    for (auto it = std::begin(v) + 1u; it != std::end(v); ++it)
        s += glue + *it;

    return s;
}

template<class String_t>
inline String_t prefix(const String_t& s1, const String_t& s2)
{
    auto len = std::min(s1.size(), s2.size());
    auto its = std::mismatch(std::begin(s1), std::begin(s1) + len, std::begin(s2));
    return String_t(std::begin(s1), its.first);
}

template<class String_t>
inline String_t replace(String_t str, const String_t& s1, const String_t& s2)
{
    auto startPos = str.find(s1, 0u);
    while (startPos != std::string::npos) {
        str.replace(startPos, s1.length(), s2);
        startPos = str.find(s1, startPos + s2.length());
    }
    return str;
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
