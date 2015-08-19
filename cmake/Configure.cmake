# Configures the main XML file.

#-----
# Version

# TODO This is duplicated with main cmake
set(VERSION_INPUT_FILE inc/core/define.hpp.in)
set(VERSION_OUTPUT_FILE inc/core/define.hpp)

# Check if repository is a git one.
if (EXISTS ${CMAKE_SOURCE_DIR}/.git)
    execute_process(COMMAND git rev-list HEAD --count
                    OUTPUT_VARIABLE EEV_VERSION_REVISION
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
# Or maybe a svn one.
else ()
    execute_process(COMMAND svnversion .
                    OUTPUT_VARIABLE EEV_VERSION_REVISION
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()

# If it does not starts with a number, there is a problem.
string(REGEX MATCH ^[0-9]+ EEV_VERSION_REVISION_EXTRACTED ${EEV_VERSION_REVISION})
if (NOT EEV_VERSION_REVISION_EXTRACTED)
    set(EEV_VERSION_REVISION_EXTRACTED NR)
    set(EEV_VERSION_REVISION NR)
endif()

configure_file(${CMAKE_SOURCE_DIR}/${VERSION_INPUT_FILE} ${CMAKE_SOURCE_DIR}/${VERSION_OUTPUT_FILE})

