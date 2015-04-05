# Configures the main XML file.

#-----
# Version

# TODO This is dulicated with main cmake
set(VERSION_INPUT_FILE inc/core/version.hpp.in)
set(VERSION_OUTPUT_FILE inc/core/version.hpp)

# Check if repository is subversion or a git one.
if (EXISTS ${CMAKE_SOURCE_DIR}/.svn)
    execute_process(COMMAND svnversion .
                    OUTPUT_VARIABLE EEV_VERSION_REVISION
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
elseif (EXISTS ${CMAKE_SOURCE_DIR}/.git)
    execute_process(COMMAND git rev-list HEAD --count
                    OUTPUT_VARIABLE EEV_VERSION_REVISION
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
else()
    # No revision information
    set(EEV_VERSION_REVISION NR)
endif()

configure_file(${CMAKE_SOURCE_DIR}/${VERSION_INPUT_FILE} ${CMAKE_SOURCE_DIR}/${VERSION_OUTPUT_FILE})

