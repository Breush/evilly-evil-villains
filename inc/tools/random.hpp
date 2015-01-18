#pragma once

//------------------------//
//----- Random class -----//

#include "tools/int.hpp"

namespace Random
{
    static inline uint32 randu32(uint32 seed, uint32 factor, uint32 in)
    {
        in |= seed * factor;
        factor = 0x9069u * (in & 0xffffu) + (in >> 16);
        in += 0x4650u * (in >> 16);
        return (factor << 16) + (in & 0xffffu);
    }

    static inline double randd(uint32 seed, uint32 factor, uint32 in)
    {
        return randu32(seed, factor, in) * 0x1.p-32;
    }

    static inline uint32 randu32(uint32 seed, uint32 factor, uint32 in, uint32 min, uint32 max)
    {
        return min + randu32(seed, factor, in) % (max - min + 1u);
    }

    static inline double randd(uint32 seed, uint32 factor, uint32 in, double min, double max)
    {
        return min + (max - min) * randd(seed, factor, in);
    }
};
