include_directories(${SPRITER_INCLUDE_DIR})

file(GLOB_RECURSE SPRITER_SRCS RELATIVE ${SPRITER_ROOT_DIR} *.cpp)

# Add library
add_library(spriterengine ${SPRITER_SRCS})

# EEV interfacing
set(SPRITER_LIBRARIES spriterengine)

