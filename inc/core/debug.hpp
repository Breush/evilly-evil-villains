#pragma once

#include "tools/debug.hpp"

//-----------------------------//
//---- Change defines here ----//

#define DEBUG_CORE  5

//--------------------//
//---- Debug mode ----//

#if DEBUG_CORE >= 1
    #define debug_core_1(...)   debug_generic(__VA_ARGS__)
    #define mdebug_core_1(...) mdebug_generic("[D1] CORE: ", __VA_ARGS__)
    #define wdebug_core_1(...) wdebug_generic(L"[D1] CORE: ", __VA_ARGS__)
#else
    #define debug_core_1(...)  ((void) 0)
    #define mdebug_core_1(...) ((void) 0)
    #define wdebug_core_1(...) ((void) 0)
#endif

#if DEBUG_CORE >= 2
    #define debug_core_2(...)   debug_generic( __VA_ARGS__)
    #define mdebug_core_2(...) mdebug_generic("[D2] CORE: ", __VA_ARGS__)
    #define wdebug_core_2(...) wdebug_generic(L"[D2] CORE: ", __VA_ARGS__)
#else
    #define debug_core_2(...)  ((void) 0)
    #define mdebug_core_2(...) ((void) 0)
    #define wdebug_core_2(...) ((void) 0)
#endif

#if DEBUG_CORE >= 3
    #define debug_core_3(...)   debug_generic(__VA_ARGS__)
    #define mdebug_core_3(...) mdebug_generic("[D3] CORE: ", __VA_ARGS__)
    #define wdebug_core_3(...) wdebug_generic(L"[D3] CORE: ", __VA_ARGS__)
#else
    #define debug_core_3(...)  ((void) 0)
    #define mdebug_core_3(...) ((void) 0)
    #define wdebug_core_3(...) ((void) 0)
#endif

#if DEBUG_CORE >= 4
    #define debug_core_4(...)   debug_generic(__VA_ARGS__)
    #define mdebug_core_4(...) mdebug_generic("[D4] CORE: ", __VA_ARGS__)
    #define wdebug_core_4(...) wdebug_generic(L"[D4] CORE: ", __VA_ARGS__)
#else
    #define debug_core_4(...)  ((void) 0)
    #define mdebug_core_4(...) ((void) 0)
    #define wdebug_core_4(...) ((void) 0)
#endif

#if DEBUG_CORE >= 5
    #define debug_core_5(...)   debug_generic(__VA_ARGS__)
    #define mdebug_core_5(...) mdebug_generic("[D5] CORE: ", __VA_ARGS__)
    #define wdebug_core_5(...) wdebug_generic(L"[D5] CORE: ", __VA_ARGS__)
#else
    #define debug_core_5(...)  ((void) 0)
    #define mdebug_core_5(...) ((void) 0)
    #define wdebug_core_5(...) ((void) 0)
#endif

