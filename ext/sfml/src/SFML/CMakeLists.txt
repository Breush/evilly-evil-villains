
# include the SFML specific macros
include(${SFML_ROOT_DIR}/cmake/Macros.cmake)

# let CMake know about our additional libraries paths
if (SFML_OS_WINDOWS)
    set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "${SFML_ROOT_DIR}/extlibs/headers")
    if(SFML_COMPILER_GCC)
        if(ARCH_32BITS)
            set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${SFML_ROOT_DIR}/extlibs/libs-mingw/x86")
            set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${SFML_ROOT_DIR}/extlibs/bin/x86")
        elseif(ARCH_64BITS)
            set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${SFML_ROOT_DIR}/extlibs/libs-mingw/x64")
            set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${SFML_ROOT_DIR}/extlibs/bin/x64")
        endif()
    elseif(SFML_COMPILER_MSVC)
        if(ARCH_32BITS)
            set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${SFML_ROOT_DIR}/extlibs/libs-msvc/x86")
        elseif(ARCH_64BITS)
            set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${SFML_ROOT_DIR}/extlibs/libs-msvc/x64")
        endif()
    endif()
elseif(SFML_OS_MACOSX)
    set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "${SFML_ROOT_DIR}/extlibs/headers")
    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${SFML_ROOT_DIR}/extlibs/libs-osx/lib/")
elseif(SFML_OS_IOS)
    set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "${SFML_ROOT_DIR}/extlibs/headers")
    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${SFML_ROOT_DIR}/extlibs/libs-ios/")
elseif(SFML_OS_ANDROID)
    set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "${SFML_ROOT_DIR}/extlibs/headers")
    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${SFML_ROOT_DIR}/extlibs/libs-android/${ANDROID_ABI}")
endif()

# add the SFML sources path
include_directories(${SFML_ROOT_DIR}/src)

# define the path of our additional CMake modules
set(CMAKE_MODULE_PATH "${SFML_ROOT_DIR}/cmake/Modules/")

# set the output directory for SFML libraries
set(LIBRARY_OUTPUT_PATH "${SFML_ROOT_DIR}/lib")

# add the modules subdirectories
add_subdirectory(System)
if(SFML_OS_WINDOWS OR SFML_OS_ANDROID OR SFML_OS_IOS)
    add_subdirectory(Main)
endif()
add_subdirectory(Window)
add_subdirectory(Network)
add_subdirectory(Graphics)
add_subdirectory(Audio)

get_directory_property(SFML_SYSTEM_LIBS DIRECTORY System DEFINITION SFML_SYSTEM_LIBS)
if(SFML_OS_WINDOWS OR SFML_OS_ANDROID OR SFML_OS_IOS)
    get_directory_property(SFML_MAIN_LIBS DIRECTORY Main DEFINITION SFML_MAIN_LIBS)
endif()
get_directory_property(SFML_WINDOW_LIBS DIRECTORY Window DEFINITION SFML_WINDOW_LIBS)
get_directory_property(SFML_NETWORK_LIBS DIRECTORY Network DEFINITION SFML_NETWORK_LIBS)
get_directory_property(SFML_GRAPHICS_LIBS DIRECTORY Graphics DEFINITION SFML_GRAPHICS_LIBS)
get_directory_property(SFML_AUDIO_LIBS DIRECTORY Audio DEFINITION SFML_AUDIO_LIBS)

set(SFML_LIBRARIES ${SFML_SYSTEM_LIBS} ${SFML_MAIN_LIBS} ${SFML_WINDOW_LIBS} ${SFML_NETWORK_LIBS} ${SFML_GRAPHICS_LIBS} ${SFML_AUDIO_LIBS})

