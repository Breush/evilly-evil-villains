#=====
# Macros

macro(eev_add_test TEST_FILE)
	string(REGEX REPLACE "(.*).cpp" "\\1" TEST ${TEST_FILE})
	add_executable(${TEST} ${TEST}.cpp)
	add_test(${TEST} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TEST})
	add_dependencies(check ${TEST})
	target_link_libraries(${TEST} ${SFML_LIBRARIES})
	target_link_libraries(${TEST} ${OPENGL_LIBRARIES})
	target_link_libraries(${TEST} ${GETTEXT_LIBRARIES})
	if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
		target_link_libraries(${TEST} ${LibIntl_LIBRARIES})
	endif ()
endmacro()

#=====
# Prologue

include_directories(${SFML_INCLUDE_DIR})
include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${GETTEXT_INCLUDE_DIR})
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
		include_directories(${LibIntl_INCLUDE_DIR})
endif()

#=====
# All tests

file(GLOB TEST_FILES RELATIVE ${CMAKE_BINARY_DIR}/tests test-*.cpp)
foreach (TEST_FILE ${TEST_FILES})
	eev_add_test(${TEST_FILE})
endforeach ()

