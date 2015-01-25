#pragma once

#ifndef DEBUG_LEVEL
    #define DEBUG_LEVEL 1
#endif

//--------------------//
//---- Debug mode ----//

#if DEBUG_LEVEL >= 1
    #define DEBUG_MODE

    #include <iostream>
    #include <cstdlib>

    #define assert(bool_expr) \
        do { if (!(bool_expr)) {\
            std::cerr << "Assertion failed: " << #bool_expr << std::endl;\
            std::cerr << "    File: " << __FILE__ << " l." << __LINE__ << std::endl;\
            std::cerr << "    Function: " << __func__ << std::endl;\
            exit(1);\
        } } while (false)

    #define massert(bool_expr, message) \
        do { if (!(bool_expr)) {\
            std::cerr << "Assertion failed: " << #bool_expr << std::endl;\
            std::cerr << "Message: " << message << std::endl;\
            std::cerr << "    File: " << __FILE__ << " l." << __LINE__ << std::endl;\
            std::cerr << "    Function: " << __func__ << std::endl;\
            exit(1);\
        } } while (false)

    #define debug(level, ...) debug_##level(__VA_ARGS__)
    #define debug_1(...) __VA_ARGS__

#if DEBUG_LEVEL >= 2
    #define debug_2(...) __VA_ARGS__
#else
    #define debug_2(...) ((void) 0)
#endif

// TODO Make a better debug system
#define debug_scml(...) debug_2(__VA_ARGS__)

//------------------//
//---- No debug ----//

#else
    #define assert(bool_expr)           ((void) 0)
    #define massert(bool_expr, message) ((void) 0)
    #define debug(level, ...)           ((void) 0)
#endif
