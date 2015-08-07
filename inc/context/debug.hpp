#pragma once

#include "tools/debug.hpp"

//-----------------------------//
//---- Change defines here ----//

#define DEBUG_CONTEXT  0

//--------------------//
//---- Debug mode ----//

#if DEBUG_CONTEXT >= 1
    #define debug_context_1(...)   debug_generic(__VA_ARGS__)
    #define mdebug_context_1(...) mdebug_generic("[D1] CONTEXT: ", __VA_ARGS__)
    #define wdebug_context_1(...) wdebug_generic(L"[D1] CONTEXT: ", __VA_ARGS__)
#else
    #define debug_context_1(...)  ((void) 0)
    #define mdebug_context_1(...) ((void) 0)
    #define wdebug_context_1(...) ((void) 0)
#endif

#if DEBUG_CONTEXT >= 2
    #define debug_context_2(...)   debug_generic( __VA_ARGS__)
    #define mdebug_context_2(...) mdebug_generic("[D2] CONTEXT: ", __VA_ARGS__)
    #define wdebug_context_2(...) wdebug_generic(L"[D2] CONTEXT: ", __VA_ARGS__)
#else
    #define debug_context_2(...)  ((void) 0)
    #define mdebug_context_2(...) ((void) 0)
    #define wdebug_context_2(...) ((void) 0)
#endif

#if DEBUG_CONTEXT >= 3
    #define debug_context_3(...)   debug_generic(__VA_ARGS__)
    #define mdebug_context_3(...) mdebug_generic("[D3] CONTEXT: ", __VA_ARGS__)
    #define wdebug_context_3(...) wdebug_generic(L"[D3] CONTEXT: ", __VA_ARGS__)
#else
    #define debug_context_3(...)  ((void) 0)
    #define mdebug_context_3(...) ((void) 0)
    #define wdebug_context_3(...) ((void) 0)
#endif

#if DEBUG_CONTEXT >= 4
    #define debug_context_4(...)   debug_generic(__VA_ARGS__)
    #define mdebug_context_4(...) mdebug_generic("[D4] CONTEXT: ", __VA_ARGS__)
    #define wdebug_context_4(...) wdebug_generic(L"[D4] CONTEXT: ", __VA_ARGS__)
#else
    #define debug_context_4(...)  ((void) 0)
    #define mdebug_context_4(...) ((void) 0)
    #define wdebug_context_4(...) ((void) 0)
#endif

#if DEBUG_CONTEXT >= 5
    #define debug_context_5(...)   debug_generic(__VA_ARGS__)
    #define mdebug_context_5(...) mdebug_generic("[D5] CONTEXT: ", __VA_ARGS__)
    #define wdebug_context_5(...) wdebug_generic(L"[D5] CONTEXT: ", __VA_ARGS__)
#else
    #define debug_context_5(...)  ((void) 0)
    #define mdebug_context_5(...) ((void) 0)
    #define wdebug_context_5(...) ((void) 0)
#endif

