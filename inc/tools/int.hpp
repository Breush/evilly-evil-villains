#pragma once

#include <cstdint>

using int8 =  int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;

using uint8 =  uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;

using uint = unsigned int;

constexpr uint8 operator"" _u8 (unsigned long long int t)
{
    return static_cast<uint8>(t);
}

constexpr int8 operator"" _i8 (unsigned long long int t)
{
    return static_cast<int8>(t);
}
