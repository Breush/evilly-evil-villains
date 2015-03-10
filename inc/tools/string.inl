
#include <algorithm> // transform
#include <cwctype>
#include <cctype>

//------------------------------//
//----- wstring <-> string -----//

inline std::wstring toWString(const std::string& s)
{
    std::wstring ws(s.size(), L' ');
    ws.resize(mbstowcs(&ws[0], s.c_str(), s.size()));
    return ws;
}

inline std::string toString(const std::wstring& ws)
{
    std::string s(ws.size(), ' ');
    s.resize(wcstombs(&s[0], ws.c_str(), ws.size()));
    return s;
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
