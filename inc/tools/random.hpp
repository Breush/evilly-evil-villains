#pragma once

#include  <random>
#include  <iterator>

namespace alea
{
    static std::random_device s_device;
    static std::mt19937 s_generator(s_device());

    //! Returns a random element from within standard iterators.
    template<typename Iter>
    inline Iter rand(Iter start, Iter end) {
        std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
        std::advance(start, dis(s_generator));
        return start;
    }

    //! Returns a random element from a standard container.
    template<typename Container>
    inline auto rand(const Container& container)
        -> decltype(*std::begin(container))
    {
        return *rand(std::begin(container), std::end(container));
    }
};
