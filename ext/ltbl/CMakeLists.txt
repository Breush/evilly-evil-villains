include_directories(${LTBL_INCLUDE_DIR})
include_directories(${LTBL_INCLUDE_DIR}/..)

file(GLOB_RECURSE LTBL_SRCS RELATIVE ${LTBL_ROOT_DIR} *.cpp)

# Add library
add_library(ltbl ${LTBL_SRCS})

# EEV interfacing
set(LTBL_LIBRARIES ltbl)

