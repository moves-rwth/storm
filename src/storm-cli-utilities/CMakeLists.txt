file(GLOB_RECURSE ALL_FILES ${PROJECT_SOURCE_DIR}/src/storm-cli-utilities/*.h ${PROJECT_SOURCE_DIR}/src/storm-cli-utilities/*.cpp)

register_source_groups_from_filestructure("${ALL_FILES}" storm-cli-utilities)



file(GLOB_RECURSE STORM_CLI_UTIL_SOURCES ${PROJECT_SOURCE_DIR}/src/storm-cli-utilities/*.cpp)
file(GLOB_RECURSE STORM_CLI_UTIL_HEADERS ${PROJECT_SOURCE_DIR}/src/storm-cli-utilities/*.h)


# Create storm-cli-utilities.
add_library(storm-cli-utilities SHARED ${STORM_CLI_UTIL_SOURCES} ${STORM_CLI_UTIL_HEADERS})

# Remove define symbol for shared libstorm.
set_target_properties(storm-cli-utilities PROPERTIES DEFINE_SYMBOL "")
#add_dependencies(storm resources)
list(APPEND STORM_TARGETS storm-cli-utilities)
set(STORM_TARGETS ${STORM_TARGETS} PARENT_SCOPE)

target_link_libraries(storm-cli-utilities PUBLIC storm storm-counterexamples storm-parsers storm-version-info)

# Install storm headers to include directory.
foreach(HEADER ${STORM_CLI_UTIL_HEADERS})
    string(REGEX REPLACE "${PROJECT_SOURCE_DIR}/src/?" "" RELATIVE_HEADER_PATH ${HEADER})
    string(REGEX MATCH "(.*)[/\\]" RELATIVE_DIRECTORY ${RELATIVE_HEADER_PATH})
    string(REGEX REPLACE "${RELATIVE_DIRECTORY}/?" "" HEADER_FILENAME ${RELATIVE_HEADER_PATH})
    add_custom_command(
            OUTPUT ${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}${HEADER_FILENAME}
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}
            COMMAND ${CMAKE_COMMAND} -E copy ${HEADER} ${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}${HEADER_FILENAME}
            DEPENDS ${HEADER}
    )
    list(APPEND STORM_CLI_UTIL_OUTPUT_HEADERS "${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}${HEADER_FILENAME}")
endforeach()
add_custom_target(copy_storm_cli_util_headers DEPENDS ${STORM_CLI_UTIL_OUTPUT_HEADERS} ${STORM_CLI_UTIL_HEADERS})
add_dependencies(storm-cli-utilities copy_storm_cli_util_headers)

# installation
install(TARGETS storm-cli-utilities EXPORT storm_Targets RUNTIME DESTINATION bin LIBRARY DESTINATION lib OPTIONAL)

