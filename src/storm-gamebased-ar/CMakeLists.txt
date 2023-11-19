file(GLOB_RECURSE ALL_FILES ${PROJECT_SOURCE_DIR}/src/storm-gamebased-ar/*.h ${PROJECT_SOURCE_DIR}/src/storm-gamebased-ar/*.cpp)

register_source_groups_from_filestructure("${ALL_FILES}" storm-gamebased-ar)

file(GLOB_RECURSE STORM_GBAR_SOURCES ${PROJECT_SOURCE_DIR}/src/storm-gamebased-ar/*/*.cpp)
file(GLOB_RECURSE STORM_GBAR_HEADERS ${PROJECT_SOURCE_DIR}/src/storm-gamebased-ar/*/*.h)

# Create storm-gamebased-ar.
add_library(storm-gamebased-ar SHARED ${STORM_GBAR_SOURCES} ${STORM_GBAR_HEADERS})

# Remove define symbol for shared libstorm.
set_target_properties(storm-gamebased-ar PROPERTIES DEFINE_SYMBOL "")
list(APPEND STORM_TARGETS storm-gamebased-ar)
set(STORM_TARGETS ${STORM_TARGETS} PARENT_SCOPE)

target_link_libraries(storm-gamebased-ar PUBLIC storm)

# Install storm headers to include directory.
foreach (HEADER ${STORM_GBAR_HEADERS})
    string(REGEX REPLACE "${PROJECT_SOURCE_DIR}/src/?" "" RELATIVE_HEADER_PATH ${HEADER})
    string(REGEX MATCH "(.*)[/\\]" RELATIVE_DIRECTORY ${RELATIVE_HEADER_PATH})
    string(REGEX REPLACE "${RELATIVE_DIRECTORY}/?" "" HEADER_FILENAME ${RELATIVE_HEADER_PATH})
    add_custom_command(
            OUTPUT ${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}${HEADER_FILENAME}
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}
            COMMAND ${CMAKE_COMMAND} -E copy ${HEADER} ${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}${HEADER_FILENAME}
            DEPENDS ${HEADER}
    )
    list(APPEND STORM_GBAR_OUTPUT_HEADERS "${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}${HEADER_FILENAME}")
endforeach ()
add_custom_target(copy_storm_gbar_headers DEPENDS ${STORM_GBAR_OUTPUT_HEADERS} ${STORM_GBAR_HEADERS})
add_dependencies(storm-gamebased-ar copy_storm_gbar_headers)

# installation
install(TARGETS storm-gamebased-ar EXPORT storm_Targets RUNTIME DESTINATION bin LIBRARY DESTINATION lib OPTIONAL)

