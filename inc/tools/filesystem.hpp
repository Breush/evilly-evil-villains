#pragma once

#include "tools/string.hpp" // toString

#include <fstream>

#if defined(__WIN32__)
    // Windows
    #include <windows.h>
#else
    // POSIX
    #include <sys/stat.h>
    #include <sys/types.h>
#endif

//! Returns true if directory was successfully created.
inline bool createDirectory(const std::wstring& dir)
{
#if defined(__WIN32__)
    // Windows
    return CreateDirectoryW(dir.c_str(), 0) != 0;
#else
    // POSIX
    return mkdir(toString(dir).c_str(), S_IRWXU | S_IRWXG | S_IRWXO) == 0;
#endif
}

//! Returns true if file exists.
inline bool fileExists(const std::wstring& file)
{
    std::ifstream fstr(toString(file));
    return fstr;
}
