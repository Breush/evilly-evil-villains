#pragma once

#include "tools/debug.hpp"

//-----------------------------//
//---- Change defines here ----//

#define DEBUG_NUI  0

//--------------------//
//---- Debug mode ----//

#if DEBUG_NUI >= 1
    #define debug_nui_1(...)   debug_generic(__VA_ARGS__)
    #define mdebug_nui_1(...) mdebug_generic("[D1] NUI: ", __VA_ARGS__)
    #define wdebug_nui_1(...) wdebug_generic(L"[D1] NUI: ", __VA_ARGS__)
#else
    #define debug_nui_1(...)  ((void) 0)
    #define mdebug_nui_1(...) ((void) 0)
    #define wdebug_nui_1(...) ((void) 0)
#endif

#if DEBUG_NUI >= 2
    #define debug_nui_2(...)   debug_generic( __VA_ARGS__)
    #define mdebug_nui_2(...) mdebug_generic("[D2] NUI: ", __VA_ARGS__)
    #define wdebug_nui_2(...) wdebug_generic(L"[D2] NUI: ", __VA_ARGS__)
#else
    #define debug_nui_2(...)  ((void) 0)
    #define mdebug_nui_2(...) ((void) 0)
    #define wdebug_nui_2(...) ((void) 0)
#endif

#if DEBUG_NUI >= 3
    #define debug_nui_3(...)   debug_generic(__VA_ARGS__)
    #define mdebug_nui_3(...) mdebug_generic("[D3] NUI: ", __VA_ARGS__)
    #define wdebug_nui_3(...) wdebug_generic(L"[D3] NUI: ", __VA_ARGS__)
#else
    #define debug_nui_3(...)  ((void) 0)
    #define mdebug_nui_3(...) ((void) 0)
    #define wdebug_nui_3(...) ((void) 0)
#endif

#if DEBUG_NUI >= 4
    #define debug_nui_4(...)   debug_generic(__VA_ARGS__)
    #define mdebug_nui_4(...) mdebug_generic("[D4] NUI: ", __VA_ARGS__)
    #define wdebug_nui_4(...) wdebug_generic(L"[D4] NUI: ", __VA_ARGS__)
#else
    #define debug_nui_4(...)  ((void) 0)
    #define mdebug_nui_4(...) ((void) 0)
    #define wdebug_nui_4(...) ((void) 0)
#endif

#if DEBUG_NUI >= 5
    #define debug_nui_5(...)   debug_generic(__VA_ARGS__)
    #define mdebug_nui_5(...) mdebug_generic("[D5] NUI: ", __VA_ARGS__)
    #define wdebug_nui_5(...) wdebug_generic(L"[D5] NUI: ", __VA_ARGS__)
#else
    #define debug_nui_5(...)  ((void) 0)
    #define mdebug_nui_5(...) ((void) 0)
    #define wdebug_nui_5(...) ((void) 0)
#endif

