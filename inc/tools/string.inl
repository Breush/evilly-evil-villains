
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

inline sf::Vector2f boundsSize(const sf::Text& text)
{
    const auto& bounds = text.getLocalBounds();
    return {bounds.left + bounds.width, bounds.top + bounds.height};
}
