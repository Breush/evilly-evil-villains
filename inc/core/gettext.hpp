#pragma once

#include <libintl.h>

#include <locale>
#include <string>
#include <vector>

inline std::wstring string2wstring(const std::string& s)
{
    std::wstring ws(s.size(), L' '); // Overestimate number of code points
    ws.resize(mbstowcs(&ws[0], s.c_str(), s.size())); // Shrink to fit

    return ws;
}

#define _(STRING) string2wstring(gettext(STRING))
