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

    // Let the user to do for (auto& x : reverse(container))

    template <typename T>
    class reverse_range
    {
    private:
        T& x_;

    public:
        reverse_range (T& x): x_ (x) {}

        inline auto begin() const -> decltype (x_.rbegin()) { return x_.rbegin(); }
        inline auto end() const -> decltype (x_.rend()) { return x_.rend(); }
    };

    template <typename T>
    reverse_range<T> reverse(T& x)
    {
        return reverse_range<T>(x);
    }
}
