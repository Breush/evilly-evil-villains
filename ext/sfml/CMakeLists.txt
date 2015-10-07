
# include the configuration file
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake)

# setup version numbers
set(VERSION_MAJOR 2)
set(VERSION_MINOR 3)
set(VERSION_PATCH 2)

# add the SFML header path
include_directories(${SFML_INCLUDE_DIR}/..)

# add an option for choosing the build type (shared or static)
set(BUILD_SHARED_LIBS TRUE)

add_subdirectory(src/SFML)

