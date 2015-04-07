#=====
# Macros

macro(eev_add_cover_program TARGET)
    add_dependencies(cover-programs ${TARGET})
    add_dependencies(${TARGET} cover-initial)
endmacro()

#=====
# Make the coverage of the tests

set(EEV_COVER_INITIAL_FILE cover-initial.info)
set(EEV_COVER_FILE cover.info)

# Clear all previous results
add_custom_target(cover-clear
                  COMMAND ${LCOV_EXECUTABLE} -q -z -d ${CMAKE_SOURCE_DIR}
                  COMMENT "Clearing previous coverage results")

# Initialize the structure
add_custom_target(cover-initial
                  COMMAND ${LCOV_EXECUTABLE} -q -c -i -d ${CMAKE_SOURCE_DIR} -o ${EEV_COVER_INITIAL_FILE}
                  COMMENT "Generating initial coverage files"
                  DEPENDS cover-clear)

# Run programs
add_custom_target(cover-programs
                  COMMENT "Running tests for coverage"
                  DEPENDS cover-initial)

# Generate the real coverage information
add_custom_target(cover-info
                  COMMAND ${LCOV_EXECUTABLE} -q -c --no-checksum -d ${CMAKE_SOURCE_DIR} -o ${EEV_COVER_FILE}
                  COMMENT "Combining coverage information"
                  DEPENDS cover-programs)

# Generate the real coverage information
add_custom_target(cover
                  COMMAND ${GENHTML_EXECUTABLE} -q -k -f -s -o html -t "EEV Coverage" ${EEV_COVER_FILE}
                  COMMENT "Generating HTML output"
                  DEPENDS cover-info)

