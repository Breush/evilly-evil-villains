#pragma once

#include <memory>

namespace std
{
    // std::make_unique does not exist in C++11 (but in C++14 it does)

    template<typename T, typename... Args>
    unique_ptr<T> make_unique(Args&&... args)
    {
        return unique_ptr<T>(new T(forward<Args>(args)...));
    }
}
