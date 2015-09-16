# Configures the main XML file.

#-----
# Version

if (EXISTS ${CMAKE_SOURCE_DIR}/.git)
    # Check if repository is a git one.
    execute_process(COMMAND git rev-list HEAD --count
                    OUTPUT_VARIABLE EEV_VERSION_REVISION
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
else ()
    # Or maybe a svn one.
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
configure_file(${CMAKE_SOURCE_DIR}/${DEBUG_INPUT_FILE} ${CMAKE_SOURCE_DIR}/${DEBUG_OUTPUT_FILE})

