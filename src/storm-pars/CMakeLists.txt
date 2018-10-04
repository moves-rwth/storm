file(GLOB_RECURSE ALL_FILES ${PROJECT_SOURCE_DIR}/src/storm-pars/*.h ${PROJECT_SOURCE_DIR}/src/storm-pars/*.cpp)

register_source_groups_from_filestructure("${ALL_FILES}" storm-pars)



file(GLOB_RECURSE STORM_PARS_SOURCES ${PROJECT_SOURCE_DIR}/src/storm-pars/*/*.cpp)
file(GLOB_RECURSE STORM_PARS_HEADERS ${PROJECT_SOURCE_DIR}/src/storm-pars/*/*.h)


# Create storm-pars.
add_library(storm-pars SHARED ${STORM_PARS_SOURCES} ${STORM_PARS_HEADERS})

# Remove define symbol for shared libstorm.
set_target_properties(storm-pars PROPERTIES DEFINE_SYMBOL "")
#add_dependencies(storm resources)
list(APPEND STORM_TARGETS storm-pars)
set(STORM_TARGETS ${STORM_TARGETS} PARENT_SCOPE)

target_link_libraries(storm-pars PUBLIC storm ${STORM_PARS_LINK_LIBRARIES})

# Install storm headers to include directory.
foreach(HEADER ${STORM_PARS_HEADERS})
	string(REGEX REPLACE "${PROJECT_SOURCE_DIR}/src/?" "" RELATIVE_HEADER_PATH ${HEADER})
	string(REGEX MATCH "(.*)[/\\]" RELATIVE_DIRECTORY ${RELATIVE_HEADER_PATH})
	string(REGEX REPLACE "${RELATIVE_DIRECTORY}/?" "" HEADER_FILENAME ${RELATIVE_HEADER_PATH})
	add_custom_command(
		OUTPUT ${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}${HEADER_FILENAME}
		COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}
		COMMAND ${CMAKE_COMMAND} -E copy ${HEADER} ${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}${HEADER_FILENAME}
		DEPENDS ${HEADER}
	)
	list(APPEND STORM_PARS_OUTPUT_HEADERS "${CMAKE_BINARY_DIR}/include/${RELATIVE_DIRECTORY}${HEADER_FILENAME}")
endforeach()
add_custom_target(copy_storm_pars_headers DEPENDS ${STORM_PARS_OUTPUT_HEADERS} ${STORM_PARS_HEADERS})
add_dependencies(storm-pars copy_storm_pars_headers)

# installation
install(TARGETS storm-pars EXPORT storm_Targets RUNTIME DESTINATION bin LIBRARY DESTINATION lib OPTIONAL)

