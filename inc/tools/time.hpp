#pragma once

#include "tools/string.hpp" // toWString

#include <chrono>
#include <ctime>
#include <string>

inline std::string time2string(const std::string& format, const time_t& rawTime)
{
    char buffer[80];
    auto timeInfo = localtime(&rawTime);
    strftime(buffer, 80, format.c_str(), timeInfo);
    return buffer;
}

inline std::wstring time2wstring(const std::string& format, const time_t& rawTime)
{
    return toWString(time2string(format, rawTime));
}

inline std::string time2string(const std::string& format)
{
    time_t rawTime;
    time(&rawTime);
    localtime(&rawTime);
    return time2string(format, rawTime);
}
