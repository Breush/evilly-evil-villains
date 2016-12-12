cmake_minimum_required(VERSION 2.8)

#=====
# Prologue

# Define a macro that helps defining an option
macro(eev_set_option var default type docstring)
    if(NOT DEFINED ${var})
        set(${var} ${default})
    endif()
    set(${var} ${${var}} CACHE ${type} ${docstring} FORCE)
endmacro()

# Useful variables
set(AUTHOR_EMAIL alexis.breust@jumpingtoasts.com)
set(EEV_VERSION_SWEET_NAME Papyri)
set(EEV_VERSION_MAJ 0)
set(EEV_VERSION_MIN 3)

#=====
# Project

# Set a default build type if none was provided
# This has to be done before the project() instruction!
eev_set_option(CMAKE_BUILD_TYPE Debug STRING "Choose the type of build (Debug or Release)")

# Add project and main executable
project(EEV)
set(EXECUTABLE_NAME eev)

# Directories
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE}")
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# Compiler options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14 -fno-exceptions")
if (CMAKE_BUILD_TYPE MATCHES "Debug")
    set(EEV_DEBUG_MODE ON)

    # Warnings flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmax-errors=2 -Wall -Wextra -Wno-unused-parameter -Wno-sign-compare")

    # GCC 5-sepcific warnings
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
        if (GCC_VERSION VERSION_GREATER 5)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsuggest-final-types -Wsuggest-final-methods")
        endif ()
    endif ()

elseif (CMAKE_BUILD_TYPE MATCHES "Release")
    set(EEV_DEBUG_MODE OFF)

    # Optimisation flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

    # Static libc++ for distribution
    if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Bstatic")
    endif ()

else ()
    # Configuration error
    message(FATAL_ERROR "Wrong CMAKE_BUILD_TYPE, should be Debug or Release")
endif ()

# Windows needs everything statically linked to be happy
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
endif ()
		
# Under Linux with GCC, enable call-stack generation
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic")
    endif ()
endif ()

# Files for project
set(CODE_MAIN_FILE src/core/main.cpp)
set(VERSION_INPUT_FILE inc/core/define.hpp.in)
set(VERSION_OUTPUT_FILE inc/core/define.hpp)
set(DEBUG_INPUT_FILE inc/tools/debug.hpp.in)
set(DEBUG_OUTPUT_FILE inc/tools/debug.hpp)
file(GLOB_RECURSE SOURCES_FILES RELATIVE ${CMAKE_BINARY_DIR} src/*.cpp)
file(GLOB_RECURSE CODE_FILES RELATIVE ${CMAKE_BINARY_DIR} src/*.cpp inc/*.hpp inc/*.hpp.in inc/*.inl res/*.vert res/*.frag res/*/ai.lua res/*/data.xml)
list(REMOVE_ITEM CODE_FILES ${CODE_MAIN_FILE} ${VERSION_OUTPUT_FILE} ${DEBUG_OUTPUT_FILE})

#=====
# Link Steam workshop

set(STEAM_ROOT_DIR ${CMAKE_SOURCE_DIR}/ext/steam)

add_subdirectory(${STEAM_ROOT_DIR})

get_directory_property(STEAM_LIBRARIES DIRECTORY ${STEAM_ROOT_DIR} DEFINITION STEAM_LIBRARIES)

#=====
# Create Box2D libraries

set(BOX2D_ROOT_DIR ${CMAKE_SOURCE_DIR}/ext/box2d)
set(BOX2D_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/ext/include/Box2D)

add_subdirectory(${BOX2D_ROOT_DIR})

get_directory_property(BOX2D_LIBRARIES DIRECTORY ${BOX2D_ROOT_DIR} DEFINITION BOX2D_LIBRARIES)

#=====
# Create Spriter libraries

set(SPRITER_ROOT_DIR ${CMAKE_SOURCE_DIR}/ext/spriter)
set(SPRITER_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/ext/include/Spriter)

add_subdirectory(${SPRITER_ROOT_DIR})

get_directory_property(SPRITER_LIBRARIES DIRECTORY ${SPRITER_ROOT_DIR} DEFINITION SPRITER_LIBRARIES)

#=====
# Create LTBL libraries

set(LTBL_ROOT_DIR ${CMAKE_SOURCE_DIR}/ext/ltbl)
set(LTBL_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/ext/include/ltbl)

add_subdirectory(${LTBL_ROOT_DIR})

get_directory_property(LTBL_LIBRARIES DIRECTORY ${LTBL_ROOT_DIR} DEFINITION LTBL_LIBRARIES)

#=====
# Create SFML libraries

set(SFML_ROOT_DIR ${CMAKE_SOURCE_DIR}/ext/sfml)
set(SFML_SOURCE_DIR ${CMAKE_SOURCE_DIR}/ext/sfml/src/SFML)
set(SFML_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/ext/include/SFML)

add_subdirectory(${SFML_ROOT_DIR})

get_directory_property(SFML_LIBRARIES DIRECTORY ${SFML_SOURCE_DIR} DEFINITION SFML_LIBRARIES)

#=====
# Create library

# We create a library so that binaries can be easily shared with other programs
set(EEV_LIBRARIES eev-common)

add_library(${EEV_LIBRARIES} ${CODE_FILES})

target_link_libraries(${EEV_LIBRARIES} ${LTBL_LIBRARIES})
target_link_libraries(${EEV_LIBRARIES} ${SFML_LIBRARIES})
target_link_libraries(${EEV_LIBRARIES} ${STEAM_LIBRARIES})
target_link_libraries(${EEV_LIBRARIES} ${BOX2D_LIBRARIES})
target_link_libraries(${EEV_LIBRARIES} ${SPRITER_LIBRARIES})

#=====
# Create game executable

eev_set_option(EEV_EXTRA_INCLUDE_PATH "" STRING "Add extra include path if needed")
include_directories(${EEV_EXTRA_INCLUDE_PATH})

include_directories(${CMAKE_SOURCE_DIR}/inc/)
include_directories(${CMAKE_SOURCE_DIR}/ext/include)

add_executable(${EXECUTABLE_NAME} ${CODE_MAIN_FILE})
target_link_libraries(${EXECUTABLE_NAME} ${EEV_LIBRARIES})

include(${CMAKE_SOURCE_DIR}/cmake/LoadEEVDependencies.cmake)

#=====
# Configure

add_custom_target(configure COMMAND ${CMAKE_COMMAND}
                  -D DEBUG_OUTPUT_FILE=${DEBUG_OUTPUT_FILE}
                  -D DEBUG_INPUT_FILE=${DEBUG_INPUT_FILE}
                  -D EEV_DEBUG_MODE=${EEV_DEBUG_MODE}
                  -D VERSION_OUTPUT_FILE=${VERSION_OUTPUT_FILE}
                  -D VERSION_INPUT_FILE=${VERSION_INPUT_FILE}
                  -D EEV_VERSION_SWEET_NAME=${EEV_VERSION_SWEET_NAME}
                  -D EEV_VERSION_MAJ=${EEV_VERSION_MAJ}
                  -D EEV_VERSION_MIN=${EEV_VERSION_MIN}
                  -P cmake/Configure.cmake
                  COMMENT "Updating configuration")
add_dependencies(${EEV_LIBRARIES} configure)

#=====
# Documentation

eev_set_option(EEV_ENABLE_DOCUMENTATION OFF BOOL "Choose whether to enable 'docs' target")
if (EEV_ENABLE_DOCUMENTATION)
    add_subdirectory(docs EXCLUDE_FROM_ALL)
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wdocumentation")
    endif ()
endif ()

#=====
# Code coverage

find_program(LCOV_EXECUTABLE lcov)
find_program(GENHTML_EXECUTABLE genhtml)

if (LCOV_EXECUTABLE AND GENHTML_EXECUTABLE)
    set(EEV_COVERAGE_ENABLED ON)
else ()
    set(EEV_COVERAGE_ENABLED OFF)
endif ()

if (EEV_COVERAGE_ENABLED)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 --coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
    add_subdirectory(cover EXCLUDE_FROM_ALL)
endif ()

#=====
# Tests

enable_testing()
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})
add_subdirectory(tests EXCLUDE_FROM_ALL)

#=====
# Translation target

# Generate the POT
find_program(XGETTEXT_EXECUTABLE xgettext)
if (NOT XGETTEXT_EXECUTABLE)
    message("Warning: xgettext not found, you won't be able to extract translatable strings from source code.")
endif ()

set(POT_FILE res/core/langs/eev.pot)
set(XGETTEXT_PARAMETERS -k_ --c++ --foreign-user -i --no-location --msgid-bugs-address=${AUTHOR_EMAIL})
set(XGETTEXT_COMMAND ${XGETTEXT_EXECUTABLE} ${XGETTEXT_PARAMETERS} -o ${POT_FILE} ${SOURCES_FILES})
add_custom_command(OUTPUT ${POT_FILE} COMMAND ${XGETTEXT_COMMAND} DEPENDS ${SOURCES_FILES})

# Generate the MOs
find_program(MSGFMT_EXECUTABLE msgfmt)
if (NOT MSGFMT_EXECUTABLE)
    message("Warning: msgfmt not found, you won't be able to generate translations binaries.")
endif ()

set(MO_FILES)
file(GLOB_RECURSE PO_FILES RELATIVE ${CMAKE_BINARY_DIR} res/core/langs/*.po)
foreach (po IN LISTS PO_FILES)
    get_filename_component(po_name_we ${po} NAME_WE)
    get_filename_component(po_directory ${po} PATH)
    set(mo ${po_directory}/${po_name_we}.mo)
    set(MSGFMT_COMMAND ${MSGFMT_EXECUTABLE} -o ${mo} ${po})
    add_custom_command(OUTPUT ${mo} COMMAND ${MSGFMT_COMMAND} DEPENDS ${po})
    list(APPEND MO_FILES ${mo})
endforeach ()

# New target
add_custom_target(translation DEPENDS ${POT_FILE} ${MO_FILES})

#=====
# Distrib target

if (NOT EEV_DEBUG_MODE)
    add_custom_target(distrib COMMAND ${CMAKE_COMMAND}
                      -D EEV_VERSION_SWEET_NAME=${EEV_VERSION_SWEET_NAME}
                      -D EEV_VERSION_MAJ=${EEV_VERSION_MAJ}
                      -D EEV_VERSION_MIN=${EEV_VERSION_MIN}
                      -D CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                      -D CMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}
                      -P cmake/Distribute.cmake
                      COMMENT "Creating archive")
    add_dependencies(distrib ${EXECUTABLE_NAME} translation)
endif ()

