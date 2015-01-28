#pragma once

#include <string>

namespace internationalization
{
    // To be called once at the beginning of the program
    void init();

    // Convert string to wstring (used by SFML)
    inline std::wstring string2wstring(const std::string& s)
    {
        std::wstring ws(s.size(), L' '); // Overestimate number of code points
        ws.resize(mbstowcs(&ws[0], s.c_str(), s.size())); // Shrink to fit

        return ws;
    }
}

#define _(STRING) internationalization::string2wstring(gettext(STRING))
