# Configures the main XML file.

#-----
# Version

# TODO This is dulicated with main cmake
set(VERSION_INPUT_FILE inc/core/version.hpp.in)
set(VERSION_OUTPUT_FILE inc/core/version.hpp)

# TODO Check if subversion or git is available.
execute_process(COMMAND svnversion -n . OUTPUT_VARIABLE EEV_VERSION_REVISION)
configure_file(${CMAKE_SOURCE_DIR}/${VERSION_INPUT_FILE} ${CMAKE_SOURCE_DIR}/${VERSION_OUTPUT_FILE})

