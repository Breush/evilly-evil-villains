#pragma once

#include "core/gettext.hpp" // string2wstring

#include <chrono>
#include <ctime>
#include <string>

#include <iostream> // FIXME

inline std::wstring time2wstring(const char* format, const time_t& rawTime)
{
    char buffer[80];
    auto timeInfo = localtime(&rawTime);
    strftime(buffer, 80, format, timeInfo);
    std::cerr << format << ": " << buffer << std::endl;
    return internationalization::string2wstring(buffer);
}
