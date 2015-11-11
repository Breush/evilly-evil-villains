#pragma once

#include "tools/int.hpp"

//! What ressources a cost is equivalent to.

template<typename T>
struct TCost
{
    T dosh = 0; //!< Dosh.
    T soul = 0; //!< Soul.
    T fame = 0; //!< Fame.
};

//! A basic cost.
using Cost = TCost<uint>;

//! A relative cost.
using RelCost = TCost<float>;
