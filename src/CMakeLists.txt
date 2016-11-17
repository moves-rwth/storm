add_subdirectory(storm)
add_subdirectory(storm-pgcl)
add_subdirectory(storm-pgcl-cli)


add_subdirectory(test)



file(GLOB_RECURSE ALL_FILES ${PROJECT_SOURCE_DIR}/src/*.h ${PROJECT_SOURCE_DIR}/src/*.cpp)

register_source_groups_from_filestructure("${ALL_FILES}")