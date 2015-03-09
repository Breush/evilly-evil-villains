#pragma once

#include "core/gettext.hpp"

#include <string>

#if defined(__WIN32__)
    // Windows
    #include <windows.h>
#else
    // POSIX
    #include <sys/stat.h>
    #include <sys/types.h>
#endif

inline bool createDirectory(const std::wstring& dir)
{
#if defined(__WIN32__)
    // Windows
    return CreateDirectoryW(dir.c_str(), 0) != 0;
#else
    // POSIX
    return mkdir(internationalization::wstring2string(dir).c_str(), S_IRWXU | S_IRWXG | S_IRWXO) == 0;
#endif
}
