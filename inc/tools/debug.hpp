#pragma once

//-----------------------------//
//---- Change defines here ----//

// Should be ON until release
#if not defined(DEBUG_GLOBAL)
    #define DEBUG_GLOBAL        1
#endif

//--------------------//
//---- Debug mode ----//

// If global off, remove all
#if DEBUG_GLOBAL == 0
    // Generic
    #define debug_generic(PRINT, ...)  ((void) 0)
    #define mdebug_generic(PRINT, ...) ((void) 0)
    #define wdebug_generic(PRINT, ...) ((void) 0)

    // Asserts
    #define assert(bool_expr)       ((void) 0)
    #define massert(bool_expr, ...) ((void) 0)
    #define wassert(bool_expr, ...) ((void) 0)

// If global on
#else
    #include <iostream>
    #include <cstdlib>

    // Generic, if LEVEL >= BASE then execute
    #define debug_generic(...) \
            __VA_ARGS__;

    // Note: std::cout and std::wcout can not be used at the same time
    // So we use std::cerr and std::wcout to handle both narrow and wide encodings
    #define mdebug_generic(PRINT, ...) \
            std::cerr << PRINT << __VA_ARGS__ << std::endl;

    #define wdebug_generic(PRINT, ...) \
            std::wcout << PRINT << __VA_ARGS__ << std::endl;

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

    #define wassert(bool_expr, ...) \
        do { if (!(bool_expr)) {\
            std::wcout << L"Assertion failed: " << #bool_expr << std::endl;\
            std::wcout << L"Message: " << __VA_ARGS__ << std::endl;\
            std::wcout << L"    File: " << __FILE__ << L" l." << __LINE__ << std::endl;\
            std::wcout << L"    Function: " << __func__ << std::endl;\
            exit(1);\
        } } while (false)
#endif

