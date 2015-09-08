# Creates a archive with all files

# Distrib directory
set(DISTRIB_DIR distrib/Evilly_Evil_Villains-${EEV_VERSION_SWEET_NAME}-${EEV_VERSION_MAJ}.${EEV_VERSION_MIN})
file(REMOVE_RECURSE ${DISTRIB_DIR})
file(MAKE_DIRECTORY ${DISTRIB_DIR})

# Resources
file(COPY res DESTINATION ${DISTRIB_DIR})

# Binaries
file(COPY bin/${CMAKE_BUILD_TYPE}/ DESTINATION ${DISTRIB_DIR})

# Librairies
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	file(COPY ext/lib/win32/ DESTINATION ${DISTRIB_DIR})
endif()

message("Exported archive to ${DISTRIB_DIR}")
