#pragma once

#include <libintl.h>
#include <cstdlib>
#include <string>

namespace internationalization
{
    // To be called once at the beginning of the program
    void init(const std::string& language);

    // Convert string to wstring (format used by SFML and pugixml)
    inline std::wstring string2wstring(const std::string& s)
    {
        std::wstring ws(s.size(), L' ');
        ws.resize(mbstowcs(&ws[0], s.c_str(), s.size()));
        return ws;
    }

    // Convert wstring to string
    inline std::string wstring2string(const std::wstring& ws)
    {
        std::string s(ws.size(), ' ');
        s.resize(wcstombs(&s[0], ws.c_str(), ws.size()));
        return s;
    }
}

#define _(STRING) internationalization::string2wstring(gettext(STRING))
