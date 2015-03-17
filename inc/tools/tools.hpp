#pragma once

// Return if inline statement
#define returnif(...) if (__VA_ARGS__) return

#include <map>

namespace tools
{
    template<typename T1, typename T2>
    inline T2 mapFind(const std::map<T1, T2>& map, T1 const& key)
    {
        const auto& element = map.find(key);
        if (element == map.end()) return T2();
        return element->second;
    }
}
