#pragma once

#include "tools/debug.hpp"

//-----------------------------//
//---- Change defines here ----//

#define DEBUG_DUNGEON  1

//--------------------//
//---- Debug mode ----//

#if DEBUG_DUNGEON >= 1
    #define debug_dungeon_1(...)   debug_generic(__VA_ARGS__)
    #define mdebug_dungeon_1(...) mdebug_generic("[D1] DUNGEON: ", __VA_ARGS__)
    #define wdebug_dungeon_1(...) wdebug_generic(L"[D1] DUNGEON: ", __VA_ARGS__)
#else
    #define debug_dungeon_1(...)  ((void) 0)
    #define mdebug_dungeon_1(...) ((void) 0)
    #define wdebug_dungeon_1(...) ((void) 0)
#endif

#if DEBUG_DUNGEON >= 2
    #define debug_dungeon_2(...)   debug_generic( __VA_ARGS__)
    #define mdebug_dungeon_2(...) mdebug_generic("[D2] DUNGEON: ", __VA_ARGS__)
    #define wdebug_dungeon_2(...) wdebug_generic(L"[D2] DUNGEON: ", __VA_ARGS__)
#else
    #define debug_dungeon_2(...)  ((void) 0)
    #define mdebug_dungeon_2(...) ((void) 0)
    #define wdebug_dungeon_2(...) ((void) 0)
#endif

#if DEBUG_DUNGEON >= 3
    #define debug_dungeon_3(...)   debug_generic(__VA_ARGS__)
    #define mdebug_dungeon_3(...) mdebug_generic("[D3] DUNGEON: ", __VA_ARGS__)
    #define wdebug_dungeon_3(...) wdebug_generic(L"[D3] DUNGEON: ", __VA_ARGS__)
#else
    #define debug_dungeon_3(...)  ((void) 0)
    #define mdebug_dungeon_3(...) ((void) 0)
    #define wdebug_dungeon_3(...) ((void) 0)
#endif

#if DEBUG_DUNGEON >= 4
    #define debug_dungeon_4(...)   debug_generic(__VA_ARGS__)
    #define mdebug_dungeon_4(...) mdebug_generic("[D4] DUNGEON: ", __VA_ARGS__)
    #define wdebug_dungeon_4(...) wdebug_generic(L"[D4] DUNGEON: ", __VA_ARGS__)
#else
    #define debug_dungeon_4(...)  ((void) 0)
    #define mdebug_dungeon_4(...) ((void) 0)
    #define wdebug_dungeon_4(...) ((void) 0)
#endif

#if DEBUG_DUNGEON >= 5
    #define debug_dungeon_5(...)   debug_generic(__VA_ARGS__)
    #define mdebug_dungeon_5(...) mdebug_generic("[D5] DUNGEON: ", __VA_ARGS__)
    #define wdebug_dungeon_5(...) wdebug_generic(L"[D5] DUNGEON: ", __VA_ARGS__)
#else
    #define debug_dungeon_5(...)  ((void) 0)
    #define mdebug_dungeon_5(...) ((void) 0)
    #define wdebug_dungeon_5(...) ((void) 0)
#endif

