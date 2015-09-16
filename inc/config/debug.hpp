#pragma once

#include "tools/debug.hpp"

//-----------------------------//
//---- Change defines here ----//

#define DEBUG_CONFIG  1

//--------------------//
//---- Debug mode ----//

#if DEBUG_CONFIG >= 1
    #define debug_config_1(...)   debug_generic(__VA_ARGS__)
    #define mdebug_config_1(...) mdebug_generic("[D1] CONFIG: ", __VA_ARGS__)
    #define wdebug_config_1(...) wdebug_generic(L"[D1] CONFIG: ", __VA_ARGS__)
#else
    #define debug_config_1(...)  ((void) 0)
    #define mdebug_config_1(...) ((void) 0)
    #define wdebug_config_1(...) ((void) 0)
#endif

#if DEBUG_CONFIG >= 2
    #define debug_config_2(...)   debug_generic( __VA_ARGS__)
    #define mdebug_config_2(...) mdebug_generic("[D2] CONFIG: ", __VA_ARGS__)
    #define wdebug_config_2(...) wdebug_generic(L"[D2] CONFIG: ", __VA_ARGS__)
#else
    #define debug_config_2(...)  ((void) 0)
    #define mdebug_config_2(...) ((void) 0)
    #define wdebug_config_2(...) ((void) 0)
#endif

#if DEBUG_CONFIG >= 3
    #define debug_config_3(...)   debug_generic(__VA_ARGS__)
    #define mdebug_config_3(...) mdebug_generic("[D3] CONFIG: ", __VA_ARGS__)
    #define wdebug_config_3(...) wdebug_generic(L"[D3] CONFIG: ", __VA_ARGS__)
#else
    #define debug_config_3(...)  ((void) 0)
    #define mdebug_config_3(...) ((void) 0)
    #define wdebug_config_3(...) ((void) 0)
#endif

#if DEBUG_CONFIG >= 4
    #define debug_config_4(...)   debug_generic(__VA_ARGS__)
    #define mdebug_config_4(...) mdebug_generic("[D4] CONFIG: ", __VA_ARGS__)
    #define wdebug_config_4(...) wdebug_generic(L"[D4] CONFIG: ", __VA_ARGS__)
#else
    #define debug_config_4(...)  ((void) 0)
    #define mdebug_config_4(...) ((void) 0)
    #define wdebug_config_4(...) ((void) 0)
#endif

#if DEBUG_CONFIG >= 5
    #define debug_config_5(...)   debug_generic(__VA_ARGS__)
    #define mdebug_config_5(...) mdebug_generic("[D5] CONFIG: ", __VA_ARGS__)
    #define wdebug_config_5(...) wdebug_generic(L"[D5] CONFIG: ", __VA_ARGS__)
#else
    #define debug_config_5(...)  ((void) 0)
    #define mdebug_config_5(...) ((void) 0)
    #define wdebug_config_5(...) ((void) 0)
#endif

