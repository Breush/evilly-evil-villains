#pragma once

#include "tools/debug.hpp"

//-----------------------------//
//---- Change defines here ----//

#define DEBUG_AI  0

//--------------------//
//---- Debug mode ----//

#if DEBUG_AI >= 1
    #define debug_ai_1(...)   debug_generic(__VA_ARGS__)
    #define mdebug_ai_1(...) mdebug_generic("[D1] AI: ", __VA_ARGS__)
    #define wdebug_ai_1(...) wdebug_generic(L"[D1] AI: ", __VA_ARGS__)
#else
    #define debug_ai_1(...)  ((void) 0)
    #define mdebug_ai_1(...) ((void) 0)
    #define wdebug_ai_1(...) ((void) 0)
#endif

#if DEBUG_AI >= 2
    #define debug_ai_2(...)   debug_generic( __VA_ARGS__)
    #define mdebug_ai_2(...) mdebug_generic("[D2] AI: ", __VA_ARGS__)
    #define wdebug_ai_2(...) wdebug_generic(L"[D2] AI: ", __VA_ARGS__)
#else
    #define debug_ai_2(...)  ((void) 0)
    #define mdebug_ai_2(...) ((void) 0)
    #define wdebug_ai_2(...) ((void) 0)
#endif

#if DEBUG_AI >= 3
    #define debug_ai_3(...)   debug_generic(__VA_ARGS__)
    #define mdebug_ai_3(...) mdebug_generic("[D3] AI: ", __VA_ARGS__)
    #define wdebug_ai_3(...) wdebug_generic(L"[D3] AI: ", __VA_ARGS__)
#else
    #define debug_ai_3(...)  ((void) 0)
    #define mdebug_ai_3(...) ((void) 0)
    #define wdebug_ai_3(...) ((void) 0)
#endif

#if DEBUG_AI >= 4
    #define debug_ai_4(...)   debug_generic(__VA_ARGS__)
    #define mdebug_ai_4(...) mdebug_generic("[D4] AI: ", __VA_ARGS__)
    #define wdebug_ai_4(...) wdebug_generic(L"[D4] AI: ", __VA_ARGS__)
#else
    #define debug_ai_4(...)  ((void) 0)
    #define mdebug_ai_4(...) ((void) 0)
    #define wdebug_ai_4(...) ((void) 0)
#endif

#if DEBUG_AI >= 5
    #define debug_ai_5(...)   debug_generic(__VA_ARGS__)
    #define mdebug_ai_5(...) mdebug_generic("[D5] AI: ", __VA_ARGS__)
    #define wdebug_ai_5(...) wdebug_generic(L"[D5] AI: ", __VA_ARGS__)
#else
    #define debug_ai_5(...)  ((void) 0)
    #define mdebug_ai_5(...) ((void) 0)
    #define wdebug_ai_5(...) ((void) 0)
#endif

