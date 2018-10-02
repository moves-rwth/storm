# Create storm-pomdp.

file(GLOB_RECURSE STORM_POMDP_CLI_SOURCES ${PROJECT_SOURCE_DIR}/src/storm-pomdp-cli/*/*.cpp)
add_executable(storm-pomdp-cli ${PROJECT_SOURCE_DIR}/src/storm-pomdp-cli/storm-pomdp.cpp ${STORM_POMDP_CLI_SOURCES})
target_link_libraries(storm-pomdp-cli storm-pomdp storm-cli-utilities) # Adding headers for xcode
set_target_properties(storm-pomdp-cli PROPERTIES OUTPUT_NAME "storm-pomdp")

add_dependencies(binaries storm-pomdp-cli)

# installation
install(TARGETS storm-pomdp-cli RUNTIME DESTINATION bin LIBRARY DESTINATION lib OPTIONAL)
