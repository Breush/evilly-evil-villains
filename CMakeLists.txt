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

# Usefull variables
set(AUTHOR_EMAIL sherlock.breust@gmail.com)

#=====
# Project

# Set a default build type if none was provided
# This has to be done before the project() instruction!
eev_set_option(CMAKE_BUILD_TYPE Debug STRING "Choose the type of build (Debug or Release)")

# Add project and main executable
project(EEV)
set(EXECUTABLE_NAME eev)

# Directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

# Compiler options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
if (CMAKE_BUILD_TYPE MATCHES "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
        if (GCC_VERSION VERSION_GREATER 5 OR GCC_VERSION VERSION_EQUAL 5)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsuggest-override")
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wsuggest-final-types -Wsuggest-final-methods" )
        endif()
    endif ()
endif ()

# Files for project
set(CODE_MAIN_FILE src/core/main.cpp)
file(GLOB_RECURSE SOURCES_FILES RELATIVE ${CMAKE_BINARY_DIR} src/*.cpp)
file(GLOB_RECURSE CODE_FILES RELATIVE ${CMAKE_BINARY_DIR} src/*.cpp inc/*.hpp inc/*.inl res/shd/*.vert res/shd/*.frag)
list(REMOVE_ITEM CODE_FILES ${CODE_MAIN_FILE})

#=====
# Create library

# We create a library so that binaries can be easily shared with tests
set(EEV_LIBRARIES eev-common)
add_library(${EEV_LIBRARIES} ${CODE_FILES})

#=====
# Create game executable

include_directories(${CMAKE_SOURCE_DIR}/inc/)
include_directories(${CMAKE_SOURCE_DIR}/ext/pugixml/)

add_executable(${EXECUTABLE_NAME} ${CODE_MAIN_FILE})
target_link_libraries(${EXECUTABLE_NAME} ${EEV_LIBRARIES})

include(${CMAKE_SOURCE_DIR}/cmake/LoadEEVDependencies.cmake)

#=====
# Documentation

eev_set_option(EEV_ENABLE_DOCUMENTATION OFF BOOL "Choose whether to enable 'docs' target")
if (EEV_ENABLE_DOCUMENTATION)
    add_subdirectory(docs EXCLUDE_FROM_ALL)
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wdocumentation")
    endif ()
endif()

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

set(POT_FILE res/po/eev.pot)
set(XGETTEXT_PARAMETERS -k_ --c++ --foreign-user -i --no-location --msgid-bugs-address=${AUTHOR_EMAIL})
set(XGETTEXT_COMMAND ${XGETTEXT_EXECUTABLE} ${XGETTEXT_PARAMETERS} -o ${POT_FILE} ${SOURCES_FILES})
add_custom_command(OUTPUT ${POT_FILE} COMMAND ${XGETTEXT_COMMAND} DEPENDS ${SOURCES_FILES})

# Generate the MOs
find_program(MSGFMT_EXECUTABLE msgfmt)
if (NOT MSGFMT_EXECUTABLE)
	message("Warning: msgfmt not found, you won't be able to generate translations binaries.")
endif ()

set(MO_FILES)
file(GLOB_RECURSE PO_FILES RELATIVE ${CMAKE_BINARY_DIR} res/po/*.po)
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

