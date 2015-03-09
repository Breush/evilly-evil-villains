#pragma once

#include "core/gettext.hpp"

#include <string>

// POSIX
#include <sys/stat.h>
#include <sys/types.h>

inline bool createDirectory(const std::wstring& dir)
{
    // POSIX
    return mkdir(internationalization::wstring2string(dir).c_str(), S_IRWXU | S_IRWXG | S_IRWXO) == 0;
}
