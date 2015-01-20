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

#=====
# Project

# Set a default build type if none was provided
# This has to be done before the project() instruction!
eev_set_option(CMAKE_BUILD_TYPE Debug STRING "Choose the type of build (Debug or Release)")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})

# Add project and includes
project(EEV)
set(EXECUTABLE_NAME eev)

file(GLOB_RECURSE SOURCES_FILES src/* inc/*)
add_executable(${EXECUTABLE_NAME} ${SOURCES_FILES})

include_directories(${CMAKE_SOURCE_DIR}/inc/)
include_directories(${CMAKE_SOURCE_DIR}/ext/pugixml/)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#=====
# Find SFML 2.X

find_package(SFML 2 REQUIRED system window graphics audio)
if(SFML_FOUND)
	include_directories(${SFML_INCLUDE_DIR})
	target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES})
endif()

#=====
# Find OpenGL

find_package(OpenGL REQUIRED)
if (OPENGL_FOUND)
	include_directories(${OPENGL_INCLUDE_DIR})
	target_link_libraries(${EXECUTABLE_NAME} ${OPENGL_LIBRARIES})
endif()

#=====
# Find Gettext

find_package(Gettext REQUIRED)
if (GETTEXT_FOUND)
	include_directories(${GETTEXT_INCLUDE_DIR})
	target_link_libraries(${EXECUTABLE_NAME} ${GETTEXT_LIBRARIES})
endif()

