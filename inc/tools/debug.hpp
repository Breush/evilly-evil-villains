#pragma once

//-----------------------------//
//---- Change defines here ----//

// Asserts and masserts - should be on until release
#define DEBUG_GLOBAL        1

// Note: For the following, max level is 5
// Basic application interactions
#define DEBUG_APPLICATION   5
// User interface
#define DEBUG_NUI           0
// SCML parsing
#define DEBUG_SCML          0
// Dungeon design
#define DEBUG_DUNGEON       2

//--------------------//
//---- Debug mode ----//

// If global off, remove all
#if DEBUG_GLOBAL == 0
    // Generic
    #define debug_generic(PRINT, ...)  ((void) 0)
    #define mdebug_generic(PRINT, ...) ((void) 0)

    // Asserts
    #define assert(bool_expr)       ((void) 0)
    #define massert(bool_expr, ...) ((void) 0)

// If global on
#else
    #include <iostream>
    #include <cstdlib>

    // Generic, if LEVEL >= BASE then execute
    #define debug_generic(PRINT, ...) \
            __VA_ARGS__;

    #define mdebug_generic(PRINT, ...) \
            std::cout << PRINT << __VA_ARGS__ << std::endl;

    // Asserts
    #define assert(bool_expr) \
        do { if (!(bool_expr)) {\
            std::cerr << "Assertion failed: " << #bool_expr << std::endl;\
            std::cerr << "    File: " << __FILE__ << " l." << __LINE__ << std::endl;\
            std::cerr << "    Function: " << __func__ << std::endl;\
            exit(1);\
        } } while (false)

    #define massert(bool_expr, ...) \
        do { if (!(bool_expr)) {\
            std::cerr << "Assertion failed: " << #bool_expr << std::endl;\
            std::cerr << "Message: " << __VA_ARGS__ << std::endl;\
            std::cerr << "    File: " << __FILE__ << " l." << __LINE__ << std::endl;\
            std::cerr << "    Function: " << __func__ << std::endl;\
            exit(1);\
        } } while (false)
#endif

//---------------------//
//---- Application ----//

#if DEBUG_APPLICATION >= 1
    #define debug_application_1(...)   debug_generic("[D1] APPLICATION: ", __VA_ARGS__)
    #define mdebug_application_1(...) mdebug_generic("[D1] APPLICATION: ", __VA_ARGS__)
#else
    #define debug_application_1(...)  ((void) 0)
    #define mdebug_application_1(...) ((void) 0)
#endif

#if DEBUG_APPLICATION >= 2
    #define debug_application_2(...)   debug_generic("[D2] APPLICATION: ", __VA_ARGS__)
    #define mdebug_application_2(...) mdebug_generic("[D2] APPLICATION: ", __VA_ARGS__)
#else
    #define debug_application_2(...)  ((void) 0)
    #define mdebug_application_2(...) ((void) 0)
#endif

#if DEBUG_APPLICATION >= 3
    #define debug_application_3(...)   debug_generic("[D3] APPLICATION: ", __VA_ARGS__)
    #define mdebug_application_3(...) mdebug_generic("[D3] APPLICATION: ", __VA_ARGS__)
#else
    #define debug_application_3(...)  ((void) 0)
    #define mdebug_application_3(...) ((void) 0)
#endif

#if DEBUG_APPLICATION >= 4
    #define debug_application_4(...)   debug_generic("[D4] APPLICATION: ", __VA_ARGS__)
    #define mdebug_application_4(...) mdebug_generic("[D4] APPLICATION: ", __VA_ARGS__)
#else
    #define debug_application_4(...)  ((void) 0)
    #define mdebug_application_4(...) ((void) 0)
#endif

#if DEBUG_APPLICATION >= 5
    #define debug_application_5(...)   debug_generic("[D5] APPLICATION: ", __VA_ARGS__)
    #define mdebug_application_5(...) mdebug_generic("[D5] APPLICATION: ", __VA_ARGS__)
#else
    #define debug_application_5(...)  ((void) 0)
    #define mdebug_application_5(...) ((void) 0)
#endif

//-------------//
//---- NUI ----//

#if DEBUG_NUI >= 1
    #define debug_nui_1(...)   debug_generic("[D1] NUI: ", __VA_ARGS__)
    #define mdebug_nui_1(...) mdebug_generic("[D1] NUI: ", __VA_ARGS__)
#else
    #define debug_nui_1(...)  ((void) 0)
    #define mdebug_nui_1(...) ((void) 0)
#endif

#if DEBUG_NUI >= 2
    #define debug_nui_2(...)   debug_generic("[D2] NUI: ", __VA_ARGS__)
    #define mdebug_nui_2(...) mdebug_generic("[D2] NUI: ", __VA_ARGS__)
#else
    #define debug_nui_2(...)  ((void) 0)
    #define mdebug_nui_2(...) ((void) 0)
#endif

#if DEBUG_NUI >= 3
    #define debug_nui_3(...)   debug_generic("[D3] NUI: ", __VA_ARGS__)
    #define mdebug_nui_3(...) mdebug_generic("[D3] NUI: ", __VA_ARGS__)
#else
    #define debug_nui_3(...)  ((void) 0)
    #define mdebug_nui_3(...) ((void) 0)
#endif

#if DEBUG_NUI >= 4
    #define debug_nui_4(...)   debug_generic("[D4] NUI: ", __VA_ARGS__)
    #define mdebug_nui_4(...) mdebug_generic("[D4] NUI: ", __VA_ARGS__)
#else
    #define debug_nui_4(...)  ((void) 0)
    #define mdebug_nui_4(...) ((void) 0)
#endif

#if DEBUG_NUI >= 5
    #define debug_nui_5(...)   debug_generic("[D5] NUI: ", __VA_ARGS__)
    #define mdebug_nui_5(...) mdebug_generic("[D5] NUI: ", __VA_ARGS__)
#else
    #define debug_nui_5(...)  ((void) 0)
    #define mdebug_nui_5(...) ((void) 0)
#endif

//--------------//
//---- SCML ----//

#if DEBUG_SCML >= 1
    #define debug_scml_1(...)   debug_generic("[D1] SCML: ", __VA_ARGS__)
    #define mdebug_scml_1(...) mdebug_generic("[D1] SCML: ", __VA_ARGS__)
#else
    #define debug_scml_1(...)  ((void) 0)
    #define mdebug_scml_1(...) ((void) 0)
#endif

#if DEBUG_SCML >= 2
    #define debug_scml_2(...)   debug_generic("[D2] SCML: ", __VA_ARGS__)
    #define mdebug_scml_2(...) mdebug_generic("[D2] SCML: ", __VA_ARGS__)
#else
    #define debug_scml_2(...)  ((void) 0)
    #define mdebug_scml_2(...) ((void) 0)
#endif

#if DEBUG_SCML >= 3
    #define debug_scml_3(...)   debug_generic("[D3] SCML: ", __VA_ARGS__)
    #define mdebug_scml_3(...) mdebug_generic("[D3] SCML: ", __VA_ARGS__)
#else
    #define debug_scml_3(...)  ((void) 0)
    #define mdebug_scml_3(...) ((void) 0)
#endif

#if DEBUG_SCML >= 4
    #define debug_scml_4(...)   debug_generic("[D4] SCML: ", __VA_ARGS__)
    #define mdebug_scml_4(...) mdebug_generic("[D4] SCML: ", __VA_ARGS__)
#else
    #define debug_scml_4(...)  ((void) 0)
    #define mdebug_scml_4(...) ((void) 0)
#endif

#if DEBUG_SCML >= 5
    #define debug_scml_5(...)   debug_generic("[D5] SCML: ", __VA_ARGS__)
    #define mdebug_scml_5(...) mdebug_generic("[D5] SCML: ", __VA_ARGS__)
#else
    #define debug_scml_5(...)  ((void) 0)
    #define mdebug_scml_5(...) ((void) 0)
#endif

//-----------------//
//---- Dungeon ----//

#if DEBUG_DUNGEON >= 1
    #define debug_dungeon_1(...)   debug_generic("[D1] DUNGEON: ", __VA_ARGS__)
    #define mdebug_dungeon_1(...) mdebug_generic("[D1] DUNGEON: ", __VA_ARGS__)
#else
    #define debug_dungeon_1(...)  ((void) 0)
    #define mdebug_dungeon_1(...) ((void) 0)
#endif

#if DEBUG_DUNGEON >= 2
    #define debug_dungeon_2(...)   debug_generic("[D2] DUNGEON: ", __VA_ARGS__)
    #define mdebug_dungeon_2(...) mdebug_generic("[D2] DUNGEON: ", __VA_ARGS__)
#else
    #define debug_dungeon_2(...)  ((void) 0)
    #define mdebug_dungeon_2(...) ((void) 0)
#endif

#if DEBUG_DUNGEON >= 3
    #define debug_dungeon_3(...)   debug_generic("[D3] DUNGEON: ", __VA_ARGS__)
    #define mdebug_dungeon_3(...) mdebug_generic("[D3] DUNGEON: ", __VA_ARGS__)
#else
    #define debug_dungeon_3(...)  ((void) 0)
    #define mdebug_dungeon_3(...) ((void) 0)
#endif

#if DEBUG_DUNGEON >= 4
    #define debug_dungeon_4(...)   debug_generic("[D4] DUNGEON: ", __VA_ARGS__)
    #define mdebug_dungeon_4(...) mdebug_generic("[D4] DUNGEON: ", __VA_ARGS__)
#else
    #define debug_dungeon_4(...)  ((void) 0)
    #define mdebug_dungeon_4(...) ((void) 0)
#endif

#if DEBUG_DUNGEON >= 5
    #define debug_dungeon_5(...)   debug_generic("[D5] DUNGEON: ", __VA_ARGS__)
    #define mdebug_dungeon_5(...) mdebug_generic("[D5] DUNGEON: ", __VA_ARGS__)
#else
    #define debug_dungeon_5(...)  ((void) 0)
    #define mdebug_dungeon_5(...) ((void) 0)
#endif
