
#include <algorithm> // transform
#include <cctype> // isalnum

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

template<typename T>
std::basic_string<T>& toLowercase(std::basic_string<T>& str)
{
    std::transform(str.begin(), str.end(), str.begin(), tolower);
    return str;
}

template<typename T>
std::basic_string<T>& filterSpecial(std::basic_string<T>& str)
{
    for (auto& c : str)
        if (!isalnum(c))
            c = '_';

    return toLowercase(str);
}
