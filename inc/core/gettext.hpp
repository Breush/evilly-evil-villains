#pragma once

#include <libintl.h>
#include <locale.h>

#include <string>
#include <vector>

inline std::wstring string2wstring(const std::string& s)
{
    // Conversion to char*, getting size
    const char * cs = s.c_str();
    const size_t wn = std::mbsrtowcs(nullptr, &cs, 0, nullptr);

    // Buffering result
    std::vector<wchar_t> buf(wn + 1);
    std::mbsrtowcs(buf.data(), &cs, wn + 1, nullptr);

    return std::wstring(buf.data(), wn);
}

#define _(STRING) string2wstring(gettext(STRING))
