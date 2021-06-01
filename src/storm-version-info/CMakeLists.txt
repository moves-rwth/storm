
file(GLOB_RECURSE ALL_FILES ${PROJECT_SOURCE_DIR}/src/storm-version-info/*.h ${PROJECT_SOURCE_DIR}/src/storm-version-info/*.cpp)
register_source_groups_from_filestructure("${ALL_FILES}" storm-version-info)

file(GLOB_RECURSE STORM_VERSION_INFO_SOURCES ${PROJECT_SOURCE_DIR}/src/storm-version-info/*.cpp)
file(GLOB_RECURSE STORM_VERSION_INFO_HEADERS ${PROJECT_SOURCE_DIR}/src/storm-version-info/*.h)

# Configure a source file to pass the Storm version to the source code
configure_file (
	"${PROJECT_SOURCE_DIR}/src/storm-version-info/storm-version.cpp.in"
	"${PROJECT_BINARY_DIR}/storm-version.cpp"
)

# Add the generated source file
list(APPEND STORM_VERSION_INFO_SOURCES "${PROJECT_BINARY_DIR}/storm-version.cpp")


# Create storm-version-info lib
add_library(storm-version-info SHARED ${STORM_VERSION_INFO_SOURCES} ${STORM_VERSION_INFO_HEADERS})

# Remove define symbol for shared libstorm.
set_target_properties(storm-version-info PROPERTIES DEFINE_SYMBOL "")

# Add dependency to core storm libary. We are not going to link against it to avoid unnecessary linking steps, but we still want to build storm-version-info as often as possible.
add_dependencies(storm storm-version-info)
list(APPEND STORM_TARGETS storm-version-info)
set(STORM_TARGETS ${STORM_TARGETS} PARENT_SCOPE)

# Install storm headers to include directory.
foreach(HEADER ${STORM_VERSION_INFO_HEADERS})
    string(REGEX REPLACE "${PROJECT_SOURCE_DIR}/src/?" "" RELATIVE_HEADER_PATH ${HEADER})
    string(REGEX MATCH "(.*)[/\\]" RELATIVE_DIRECTORY ${RELATIVE_HEADER_PATH})
    string(REGEX REPLACE "${RELATIVE_DIRECTORY}/?" "" HEADER_FILENAME ${RELATIVE_HEADER_PATH})
    add_custom_command(
            OUTPUT ${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}${HEADER_FILENAME}
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}
            COMMAND ${CMAKE_COMMAND} -E copy ${HEADER} ${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}${HEADER_FILENAME}
            DEPENDS ${HEADER}
    )
    list(APPEND STORM_VERSION_INFO_OUTPUT_HEADERS "${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}${HEADER_FILENAME}")
endforeach()
add_custom_target(copy_storm_version_info_headers DEPENDS ${STORM_VERSION_INFO_OUTPUT_HEADERS} ${STORM_VERSION_INFO_HEADERS})
add_dependencies(storm-version-info copy_storm_version_info_headers)

# installation
install(TARGETS storm-version-info EXPORT storm_Targets RUNTIME DESTINATION bin LIBRARY DESTINATION lib OPTIONAL)

