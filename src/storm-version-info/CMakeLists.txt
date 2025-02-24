file(GLOB_RECURSE STORM_VERSION_INFO_SOURCES ${PROJECT_SOURCE_DIR}/src/storm-version-info/*.cpp)
file(GLOB_RECURSE STORM_VERSION_INFO_HEADERS RELATIVE "${PROJECT_SOURCE_DIR}/src/storm-version-info" ${PROJECT_SOURCE_DIR}/src/storm-version-info/*.h)

# Configure a source file to pass the Storm version to the source code
configure_file (
	"${PROJECT_SOURCE_DIR}/src/storm-version-info/storm-version.cpp.in"
	"${PROJECT_BINARY_DIR}/storm-version.cpp"
)

# Add the generated source file
list(APPEND STORM_VERSION_INFO_SOURCES "${PROJECT_BINARY_DIR}/storm-version.cpp")

# Create storm-version-info lib
add_library(storm-version-info SHARED)
target_sources(storm-version-info
		PRIVATE
		${STORM_VERSION_INFO_SOURCES}
		PUBLIC
		FILE_SET fs_storm_version_info_headers TYPE HEADERS BASE_DIRS "${PROJECT_SOURCE_DIR}/src" FILES ${STORM_VERSION_INFO_HEADERS})
# Add dependency to core storm libary.
# We are not going to link against it to avoid unnecessary linking steps,
# but we still want to build storm-version-info as often as possible.
add_dependencies(storm storm-version-info)

list(APPEND STORM_TARGETS storm-version-info)
set(STORM_TARGETS ${STORM_TARGETS} PARENT_SCOPE)

# installation
install(TARGETS storm-version-info EXPORT storm_Targets RUNTIME DESTINATION bin LIBRARY DESTINATION lib OPTIONAL FILE_SET fs_storm_version_info_headers)

