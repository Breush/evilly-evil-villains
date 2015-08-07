#pragma once

#include "tools/debug.hpp"

//-----------------------------//
//---- Change defines here ----//

#define DEBUG_DCB  1

//--------------------//
//---- Debug mode ----//

#if DEBUG_DCB >= 1
    #define debug_dcb_1(...)   debug_generic(__VA_ARGS__)
    #define mdebug_dcb_1(...) mdebug_generic("[D1] DCB: ", __VA_ARGS__)
    #define wdebug_dcb_1(...) wdebug_generic(L"[D1] DCB: ", __VA_ARGS__)
#else
    #define debug_dcb_1(...)  ((void) 0)
    #define mdebug_dcb_1(...) ((void) 0)
    #define wdebug_dcb_1(...) ((void) 0)
#endif

#if DEBUG_DCB >= 2
    #define debug_dcb_2(...)   debug_generic( __VA_ARGS__)
    #define mdebug_dcb_2(...) mdebug_generic("[D2] DCB: ", __VA_ARGS__)
    #define wdebug_dcb_2(...) wdebug_generic(L"[D2] DCB: ", __VA_ARGS__)
#else
    #define debug_dcb_2(...)  ((void) 0)
    #define mdebug_dcb_2(...) ((void) 0)
    #define wdebug_dcb_2(...) ((void) 0)
#endif

#if DEBUG_DCB >= 3
    #define debug_dcb_3(...)   debug_generic(__VA_ARGS__)
    #define mdebug_dcb_3(...) mdebug_generic("[D3] DCB: ", __VA_ARGS__)
    #define wdebug_dcb_3(...) wdebug_generic(L"[D3] DCB: ", __VA_ARGS__)
#else
    #define debug_dcb_3(...)  ((void) 0)
    #define mdebug_dcb_3(...) ((void) 0)
    #define wdebug_dcb_3(...) ((void) 0)
#endif

#if DEBUG_DCB >= 4
    #define debug_dcb_4(...)   debug_generic(__VA_ARGS__)
    #define mdebug_dcb_4(...) mdebug_generic("[D4] DCB: ", __VA_ARGS__)
    #define wdebug_dcb_4(...) wdebug_generic(L"[D4] DCB: ", __VA_ARGS__)
#else
    #define debug_dcb_4(...)  ((void) 0)
    #define mdebug_dcb_4(...) ((void) 0)
    #define wdebug_dcb_4(...) ((void) 0)
#endif

#if DEBUG_DCB >= 5
    #define debug_dcb_5(...)   debug_generic(__VA_ARGS__)
    #define mdebug_dcb_5(...) mdebug_generic("[D5] DCB: ", __VA_ARGS__)
    #define wdebug_dcb_5(...) wdebug_generic(L"[D5] DCB: ", __VA_ARGS__)
#else
    #define debug_dcb_5(...)  ((void) 0)
    #define mdebug_dcb_5(...) ((void) 0)
    #define wdebug_dcb_5(...) ((void) 0)
#endif

