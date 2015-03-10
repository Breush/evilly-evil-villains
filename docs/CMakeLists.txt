#=====
# Find doxygen

find_package(Doxygen REQUIRED)

#=====
# Configure

# Set the input and output documentation paths
set(DOXYGEN_INPUT_DIR ${PROJECT_SOURCE_DIR})
set(DOXYGEN_OUTPUT_DIR ${PROJECT_BINARY_DIR}/docs)

# No CHM generation (common help used on Windows)
set(DOXYGEN_GENERATE_HTMLHELP NO)
set(DOXYGEN_HHC_PROGRAM)

# Configure the source Doxyfile by copying it and replacing all @variables@
file(GLOB_RECURSE DOXYGEN_INPUT ${PROJECT_SOURCE_DIR}/inc/*.hpp)
string (REPLACE ";" " " DOXYGEN_INPUT "${DOXYGEN_INPUT}")
set(DOXYGEN_CONFIGURED_INPUT ${DOXYGEN_OUTPUT_DIR}/doxyfile)
configure_file(${DOXYGEN_INPUT_DIR}/docs/doxyfile.in ${DOXYGEN_CONFIGURED_INPUT} @ONLY)

# Copy the files needed by the documentation
configure_file(${DOXYGEN_INPUT_DIR}/docs/doxygen.css ${DOXYGEN_OUTPUT_DIR}/html/doxygen.css COPYONLY)

#=====
# Target

add_custom_target(docs ALL
                  COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_CONFIGURED_INPUT}
                  WORKING_DIRECTORY ${DOXYGEN_INPUT_DIR})

