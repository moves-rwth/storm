file(GLOB_RECURSE ALL_FILES ${PROJECT_SOURCE_DIR}/src/storm-conv/*.h ${PROJECT_SOURCE_DIR}/src/storm-conv/*.cpp)

register_source_groups_from_filestructure("${ALL_FILES}" storm-conv)



file(GLOB_RECURSE STORM_CONV_SOURCES ${PROJECT_SOURCE_DIR}/src/storm-conv/*/*.cpp)
file(GLOB_RECURSE STORM_CONV_HEADERS ${PROJECT_SOURCE_DIR}/src/storm-conv/*/*.h)


# Create storm-conv.
add_library(storm-conv SHARED ${STORM_CONV_SOURCES} ${STORM_CONV_HEADERS})

# Remove define symbol for shared libstorm.
set_target_properties(storm-conv PROPERTIES DEFINE_SYMBOL "")
#add_dependencies(storm resources)
list(APPEND STORM_TARGETS storm-conv)
set(STORM_TARGETS ${STORM_TARGETS} PARENT_SCOPE)

target_link_libraries(storm-conv PUBLIC storm ${STORM_CONV_LINK_LIBRARIES})

# Install storm headers to include directory.
foreach(HEADER ${STORM_CONV_HEADERS})
	string(REGEX REPLACE "${PROJECT_SOURCE_DIR}/src/?" "" RELATIVE_HEADER_PATH ${HEADER})
	string(REGEX MATCH "(.*)[/\\]" RELATIVE_DIRECTORY ${RELATIVE_HEADER_PATH})
	string(REGEX REPLACE "${RELATIVE_DIRECTORY}/?" "" HEADER_FILENAME ${RELATIVE_HEADER_PATH})
	add_custom_command(
		OUTPUT ${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}${HEADER_FILENAME}
		COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}
		COMMAND ${CMAKE_COMMAND} -E copy ${HEADER} ${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}${HEADER_FILENAME}
		DEPENDS ${HEADER}
	)
	list(APPEND STORM_CONV_OUTPUT_HEADERS "${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}${HEADER_FILENAME}")
endforeach()
add_custom_target(copy_storm_conv_headers DEPENDS ${STORM_CONV_OUTPUT_HEADERS} ${STORM_CONV_HEADERS})
add_dependencies(storm-conv copy_storm_conv_headers)

# installation
install(TARGETS storm-conv EXPORT storm_Targets RUNTIME DESTINATION bin LIBRARY DESTINATION lib OPTIONAL)

