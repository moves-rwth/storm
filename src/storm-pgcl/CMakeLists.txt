
file(GLOB_RECURSE STORM_PGCL_SOURCES ${PROJECT_SOURCE_DIR}/src/storm-pgcl/*/*.cpp)
file(GLOB_RECURSE STORM_PGCL_HEADERS ${PROJECT_SOURCE_DIR}/src/storm-pgcl/*/*.h)


# Create storm-pgcl.
add_library(storm-pgcl SHARED ${STORM_PGCL_SOURCES} ${STORM_PGCL_HEADERS})
target_link_libraries(storm-pgcl storm)
