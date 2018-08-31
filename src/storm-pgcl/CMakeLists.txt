file(GLOB_RECURSE ALL_FILES ${PROJECT_SOURCE_DIR}/src/storm-pgcl/*.h ${PROJECT_SOURCE_DIR}/src/storm-pgcl/*.cpp)

register_source_groups_from_filestructure("${ALL_FILES}" storm-pgcl)



file(GLOB_RECURSE STORM_PGCL_SOURCES ${PROJECT_SOURCE_DIR}/src/storm-pgcl/*/*.cpp)
file(GLOB_RECURSE STORM_PGCL_HEADERS ${PROJECT_SOURCE_DIR}/src/storm-pgcl/*/*.h)


# Create storm-pgcl.
add_library(storm-pgcl SHARED ${STORM_PGCL_SOURCES} ${STORM_PGCL_HEADERS})
target_link_libraries(storm-pgcl storm storm-parsers storm-conv)

# installation
install(TARGETS storm-pgcl EXPORT storm_Targets RUNTIME DESTINATION bin LIBRARY DESTINATION lib OPTIONAL)
