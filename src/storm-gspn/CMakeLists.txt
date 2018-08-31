file(GLOB_RECURSE ALL_FILES ${PROJECT_SOURCE_DIR}/src/storm-gspn/*.h ${PROJECT_SOURCE_DIR}/src/storm-gspn/*.cpp)

register_source_groups_from_filestructure("${ALL_FILES}" storm-gspn)



file(GLOB_RECURSE STORM_GSPN_SOURCES ${PROJECT_SOURCE_DIR}/src/storm-gspn/*/*.cpp)
file(GLOB_RECURSE STORM_GSPN_HEADERS ${PROJECT_SOURCE_DIR}/src/storm-gspn/*/*.h)


# Create storm-gspn.
add_library(storm-gspn SHARED ${STORM_GSPN_SOURCES} ${STORM_GSPN_HEADERS})

# Remove define symbol for shared libstorm.
set_target_properties(storm-gspn PROPERTIES DEFINE_SYMBOL "")
#add_dependencies(storm resources)
list(APPEND STORM_TARGETS storm-gspn)
set(STORM_TARGETS ${STORM_TARGETS} PARENT_SCOPE)
target_link_libraries(storm-gspn PUBLIC storm storm-conv storm-parsers ${STORM_GSPN_LINK_LIBRARIES})

# Install storm headers to include directory.
foreach(HEADER ${STORM_GSPN_HEADERS})
	string(REGEX REPLACE "${PROJECT_SOURCE_DIR}/src/?" "" RELATIVE_HEADER_PATH ${HEADER})
	string(REGEX MATCH "(.*)[/\\]" RELATIVE_DIRECTORY ${RELATIVE_HEADER_PATH})
	string(REGEX REPLACE "${RELATIVE_DIRECTORY}/?" "" HEADER_FILENAME ${RELATIVE_HEADER_PATH})
	add_custom_command(
		OUTPUT ${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}${HEADER_FILENAME}
		COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}
		COMMAND ${CMAKE_COMMAND} -E copy ${HEADER} ${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}${HEADER_FILENAME}
		DEPENDS ${HEADER}
	)
	list(APPEND STORM_GSPN_OUTPUT_HEADERS "${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}${HEADER_FILENAME}")
endforeach()
add_custom_target(copy_storm_gspn_headers DEPENDS ${STORM_GSPN_OUTPUT_HEADERS} ${STORM_GSPN_HEADERS})
add_dependencies(storm-gspn copy_storm_gspn_headers)

# installation
install(TARGETS storm-gspn EXPORT storm_Targets RUNTIME DESTINATION bin LIBRARY DESTINATION lib OPTIONAL)
