add_executable(storm-pgcl-cli ${PROJECT_SOURCE_DIR}/src/storm-pgcl-cli/storm-pgcl.cpp)
target_link_libraries(storm-pgcl-cli storm-pgcl  storm-cli-utilities)
set_target_properties(storm-pgcl-cli PROPERTIES OUTPUT_NAME "storm-pgcl")

# installation
install(TARGETS storm-pgcl-cli RUNTIME DESTINATION bin LIBRARY DESTINATION lib OPTIONAL)