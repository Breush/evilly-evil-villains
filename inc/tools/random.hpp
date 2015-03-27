#pragma once

#include <random>
#include <iterator>

namespace alea
{
    static std::mt19937 s_generator;

    //! Returns a random element from within standard iterators.
    template<typename Iter>
    inline Iter rand(Iter start, Iter end)
    {
        using difference_type = typename std::iterator_traits<Iter>::difference_type;
        std::uniform_int_distribution<difference_type>
            distribution(static_cast<difference_type>(0),
            std::distance(start, end) - static_cast<difference_type>(1));
        return start + distribution(s_generator);
    }

    //! Returns a random element from a standard container.
    template<typename Container>
    inline auto rand(const Container& container)
        -> decltype(*std::begin(container))
    {
        return *rand(std::begin(container), std::end(container));
    }
};
