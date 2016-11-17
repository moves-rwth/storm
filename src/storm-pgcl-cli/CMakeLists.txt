add_executable(storm-pgcl-cli ${PROJECT_SOURCE_DIR}/src/storm-pgcl-cli/storm-pgcl.cpp)
target_link_libraries(storm-pgcl-cli storm-pgcl)
set_target_properties(storm-main PROPERTIES OUTPUT_NAME "storm-pgcl")