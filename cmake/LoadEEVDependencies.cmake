# ${EXECUTABLE_NAME} should be set before calling

#=====
# Add DL (for call-stack dynamic check)

target_link_libraries(${EXECUTABLE_NAME} dl)

#=====
# Find Lua

find_package(Lua REQUIRED)
if (LUA_FOUND)
	include_directories(${LUA_INCLUDE_DIR})
	target_link_libraries(${EXECUTABLE_NAME} ${LUA_LIBRARIES})
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
