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

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(${CMAKE_SOURCE_DIR}/inc/)
include_directories(${CMAKE_SOURCE_DIR}/ext/pugixml/)

file(GLOB_RECURSE SOURCES_FILES src/*)
file(GLOB_RECURSE CODE_FILES src/* inc/* res/shd/*)
add_executable(${EXECUTABLE_NAME} ${CODE_FILES})

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

set(POT_FILE ${CMAKE_BINARY_DIR}/res/po/eev.pot)
set(XGETTEXT_PARAMETERS "-k_ --c++ --from-code=UTF-8 --foreign-user -i --no-location --msgid-bugs-address=${AUTHOR_EMAIL}")
set(XGETTEXT_COMMAND ${XGETTEXT_EXECUTABLE} ${XGETTEXT_PARAMETERS} -o ${POT_FILE} ${SOURCES_FILES})
add_custom_command(OUTPUT ${POT_FILE} COMMAND ${XGETTEXT_COMMAND} DEPENDS ${SOURCES_FILES})

# Generate the MOs
find_program(MSGFMT_EXECUTABLE msgfmt)
if (NOT MSGFMT_EXECUTABLE)
	message("Warning: msgfmt not found, you won't be able to generate translations binaries.")
endif ()

set(MO_FILES)
file(GLOB_RECURSE PO_FILES res/po/*.po)
foreach (po IN LISTS PO_FILES)
	get_filename_component(po_name_we ${po} NAME_WE)
	get_filename_component(po_directory ${po} DIRECTORY)
	set(mo ${po_directory}/${po_name_we}.mo)
	set(MSGFMT_COMMAND ${MSGFMT_EXECUTABLE} -o ${mo} ${po})
	add_custom_command(OUTPUT ${mo} COMMAND ${MSGFMT_COMMAND} DEPENDS ${po})
	list(APPEND MO_FILES ${mo})
endforeach ()

# New target
add_custom_target(translation DEPENDS ${POT_FILE} ${MO_FILES})

#=====
# Find SFML 2.X

eev_set_option(SFML_ROOT "" STRING "Where is SFML installed")
find_package(SFML 2 REQUIRED system window graphics audio)
if (SFML_FOUND)
	include_directories(${SFML_INCLUDE_DIR})
	target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES})
endif ()

#=====
# Find OpenGL

find_package(OpenGL REQUIRED)
if (OPENGL_FOUND)
	include_directories(${OPENGL_INCLUDE_DIR})
	target_link_libraries(${EXECUTABLE_NAME} ${OPENGL_LIBRARIES})
endif ()

#=====
# Find Gettext

find_package(Gettext REQUIRED)
if (GETTEXT_FOUND)
	include_directories(${GETTEXT_INCLUDE_DIR})
	target_link_libraries(${EXECUTABLE_NAME} ${GETTEXT_LIBRARIES})
endif ()

#=====
# Find LibIntl (comes with gettext on Linux)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	find_package(LibIntl REQUIRED)
	if (LibIntl_FOUND)
		include_directories(${LibIntl_INCLUDE_DIR})
		target_link_libraries(${EXECUTABLE_NAME} ${LibIntl_LIBRARIES})
	endif ()
endif ()

