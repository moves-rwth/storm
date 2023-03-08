file(GLOB_RECURSE ALL_FILES ${PROJECT_SOURCE_DIR}/src/storm-permissive/*.h ${PROJECT_SOURCE_DIR}/src/storm-permissive/*.cpp)

register_source_groups_from_filestructure("${ALL_FILES}" storm-permissive)

file(GLOB_RECURSE STORM_PERMISSIVE_SOURCES ${PROJECT_SOURCE_DIR}/src/storm-permissive/*/*.cpp)
file(GLOB_RECURSE STORM_PERMISSIVE_HEADERS ${PROJECT_SOURCE_DIR}/src/storm-permissive/*/*.h)

# Create storm-permissive.
add_library(storm-permissive SHARED ${STORM_PERMISSIVE_SOURCES} ${STORM_PERMISSIVE_HEADERS})

# Remove define symbol for shared libstorm.
set_target_properties(storm-permissive PROPERTIES DEFINE_SYMBOL "")
list(APPEND STORM_TARGETS storm-permissive)
set(STORM_TARGETS ${STORM_TARGETS} PARENT_SCOPE)

target_link_libraries(storm-permissive PUBLIC storm)

# Install storm headers to include directory.
foreach (HEADER ${STORM_PERMISSIVE_HEADERS})
    string(REGEX REPLACE "${PROJECT_SOURCE_DIR}/src/?" "" RELATIVE_HEADER_PATH ${HEADER})
    string(REGEX MATCH "(.*)[/\\]" RELATIVE_DIRECTORY ${RELATIVE_HEADER_PATH})
    string(REGEX REPLACE "${RELATIVE_DIRECTORY}/?" "" HEADER_FILENAME ${RELATIVE_HEADER_PATH})
    add_custom_command(
            OUTPUT ${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}${HEADER_FILENAME}
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}
            COMMAND ${CMAKE_COMMAND} -E copy ${HEADER} ${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}${HEADER_FILENAME}
            DEPENDS ${HEADER}
    )
    list(APPEND STORM_PERMISSIVE_OUTPUT_HEADERS "${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}${HEADER_FILENAME}")
endforeach ()
add_custom_target(copy_storm_permissive_headers DEPENDS ${STORM_PERMISSIVE_OUTPUT_HEADERS} ${STORM_PERMISSIVE_HEADERS})
add_dependencies(storm-permissive copy_storm_permissive_headers)

# installation
install(TARGETS storm-permissive EXPORT storm_Targets RUNTIME DESTINATION bin LIBRARY DESTINATION lib OPTIONAL)

