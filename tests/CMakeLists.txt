#=====
# Code coverage for tests

if (EEV_COVERAGE_ENABLED)
    add_custom_target(cover-check COMMAND ${CMAKE_CTEST_COMMAND} -Q)
    eev_add_cover_program(cover-check)
endif ()

#=====
# Macros

macro(eev_add_test TEST_FILE)
    get_filename_component(TEST ${TEST_FILE} NAME_WE)
    add_executable(${TEST} ${TEST_FILE})
    add_test(NAME ${TEST} COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TEST} 
             WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    
    # Dependencies
    add_dependencies(check ${TEST})
    if (EEV_COVERAGE_ENABLED)
        add_dependencies(cover-check ${TEST})
    endif ()

    # Libraries
    target_link_libraries(${TEST} ${EEV_LIBRARIES})
    target_link_libraries(${TEST} ${SFML_LIBRARIES})
    target_link_libraries(${TEST} ${OPENGL_LIBRARIES})
    target_link_libraries(${TEST} ${GETTEXT_LIBRARIES})
    target_link_libraries(${TEST} ${LUA_LIBRARIES})
    if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        target_link_libraries(${TEST} ${LibIntl_LIBRARIES})
    endif ()
endmacro()

#=====
# All tests

file(GLOB TEST_FILES RELATIVE ${CMAKE_BINARY_DIR}/tests test-*.cpp debug-*.cpp)
foreach (TEST_FILE ${TEST_FILES})
    eev_add_test(${TEST_FILE})
endforeach ()

#=====
# Specificities

add_dependencies(test-gettext translation)

