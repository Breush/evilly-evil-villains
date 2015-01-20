#pragma once

// Fix for MinGW - GCC not getting std::stoi()
// See http://stackoverflow.com/questions/8542221/stdstoi-doesnt-exist-in-g-4-6-1-on-mingw

#ifdef __GNUC__
#ifdef __MINGW32__

#include <sstream>

namespace std
{
    int stoi(const std::string& str)
    {
        std::istringstream is(str);
        int result;
        is >> result;
        return result;
    }
}

#endif // __MINGW32__
#endif // __GNUC__
