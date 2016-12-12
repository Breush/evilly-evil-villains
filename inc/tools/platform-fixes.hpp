#pragma once

#include <memory>
#include <algorithm>

namespace std
{
    #if __cplusplus < 201402L

        // std::make_unique does not exist in C++11 (but in C++14 it does)

        template<typename T, typename... Args>
        unique_ptr<T> make_unique(Args&&... args)
        {
            return unique_ptr<T>(new T(forward<Args>(args)...));
        }

    #endif

    // std::find_if with container directly passed

    template<typename T, typename F>
    auto find_if(T& container, const F& func) -> decltype(std::end(container))
    {
        return std::find_if(std::begin(container), std::end(container), func);
    }

    // std::erase_if is like a remove_if but really erase the container

    template<typename T, typename F>
    void erase_if(T& container, const F& func)
    {
        auto newEnd = std::remove_if(std::begin(container), std::end(container), func);
        container.erase(newEnd, std::end(container));
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
