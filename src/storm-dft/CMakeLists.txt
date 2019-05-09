file(GLOB_RECURSE ALL_FILES ${PROJECT_SOURCE_DIR}/src/storm-dft/*.h ${PROJECT_SOURCE_DIR}/src/storm-dft/*.cpp)

register_source_groups_from_filestructure("${ALL_FILES}" storm-dft)



file(GLOB_RECURSE STORM_DFT_SOURCES ${PROJECT_SOURCE_DIR}/src/storm-dft/*/*.cpp)
file(GLOB_RECURSE STORM_DFT_HEADERS ${PROJECT_SOURCE_DIR}/src/storm-dft/*/*.h)


# Create storm-dft.
add_library(storm-dft SHARED ${STORM_DFT_SOURCES} ${STORM_DFT_HEADERS})

# Remove define symbol for shared libstorm.
set_target_properties(storm-dft PROPERTIES DEFINE_SYMBOL "")
#add_dependencies(storm resources)
list(APPEND STORM_TARGETS storm-dft)
set(STORM_TARGETS ${STORM_TARGETS} PARENT_SCOPE)

target_link_libraries(storm-dft PUBLIC storm storm-gspn storm-conv storm-parsers ${STORM_DFT_LINK_LIBRARIES})

# Install storm headers to include directory.
foreach(HEADER ${STORM_DFT_HEADERS})
	string(REGEX REPLACE "${PROJECT_SOURCE_DIR}/src/?" "" RELATIVE_HEADER_PATH ${HEADER})
	string(REGEX MATCH "(.*)[/\\]" RELATIVE_DIRECTORY ${RELATIVE_HEADER_PATH})
	string(REGEX REPLACE "${RELATIVE_DIRECTORY}/?" "" HEADER_FILENAME ${RELATIVE_HEADER_PATH})
	add_custom_command(
		OUTPUT ${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}${HEADER_FILENAME}
		COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}
		COMMAND ${CMAKE_COMMAND} -E copy ${HEADER} ${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}${HEADER_FILENAME}
		DEPENDS ${HEADER}
	)
	list(APPEND STORM_DFT_OUTPUT_HEADERS "${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}${HEADER_FILENAME}")
endforeach()
add_custom_target(copy_storm_dft_headers DEPENDS ${STORM_DFT_OUTPUT_HEADERS} ${STORM_DFT_HEADERS})
add_dependencies(storm-dft copy_storm_dft_headers)

# installation
install(TARGETS storm-dft EXPORT storm_Targets RUNTIME DESTINATION bin LIBRARY DESTINATION lib OPTIONAL)

