
# Add linux dependencies
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    if(CMAKE_SIZEOF_VOID_P EQUAL 4)
		set(STEAM_LIBRARIES ${STEAM_ROOT_DIR}/linux32/libsteam_api.so)
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(STEAM_LIBRARIES ${STEAM_ROOT_DIR}/linux64/libsteam_api.so)
    endif()

# Add MacOS dependencies
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(STEAM_LIBRARIES ${STEAM_ROOT_DIR}/osx32/libsteam_api.dylib)

# Add Windows dependencies
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	# 32-bits will work on both 32 and 64 bits systems
	set(STEAM_LIBRARIES ${STEAM_ROOT_DIR}/win32/steam_api.lib)
	
	file(COPY win32/steam_api.dll
		DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()
