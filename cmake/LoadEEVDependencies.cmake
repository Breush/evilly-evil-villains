# ${EXECUTABLE_NAME} should be set before calling

#=====
# Find SFML 2.X

eev_set_option(SFML_ROOT "" STRING "Where is SFML installed")
find_package(SFML 2 REQUIRED system window graphics audio)
if (SFML_FOUND)
	include_directories(${SFML_INCLUDE_DIR})
	target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES})
endif ()

#=====
# Find OpenGL

find_package(OpenGL REQUIRED)
if (OPENGL_FOUND)
	include_directories(${OPENGL_INCLUDE_DIR})
	target_link_libraries(${EXECUTABLE_NAME} ${OPENGL_LIBRARIES})
endif ()

#=====
# Find Gettext

find_package(Gettext REQUIRED)
if (GETTEXT_FOUND)
	include_directories(${GETTEXT_INCLUDE_DIR})
	target_link_libraries(${EXECUTABLE_NAME} ${GETTEXT_LIBRARIES})
endif ()

#=====
# Find LibIntl (comes with gettext on Linux)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	find_package(LibIntl REQUIRED)
	if (LibIntl_FOUND)
		include_directories(${LibIntl_INCLUDE_DIR})
		target_link_libraries(${EXECUTABLE_NAME} ${LibIntl_LIBRARIES})
	endif ()
endif ()
