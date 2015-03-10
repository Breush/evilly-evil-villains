#pragma once

#include "tools/string.hpp" // toWString

#include <chrono>
#include <ctime>
#include <string>

inline std::wstring time2wstring(const char* format, const time_t& rawTime)
{
    char buffer[80];
    auto timeInfo = localtime(&rawTime);
    strftime(buffer, 80, format, timeInfo);
    return toWString(buffer);
}
