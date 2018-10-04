# Create storm-conv.
add_executable(storm-conv-cli ${PROJECT_SOURCE_DIR}/src/storm-conv-cli/storm-conv.cpp)
target_link_libraries(storm-conv-cli storm-conv storm-cli-utilities) # Adding headers for xcode
set_target_properties(storm-conv-cli PROPERTIES OUTPUT_NAME "storm-conv")

add_dependencies(binaries storm-conv-cli)

# installation
install(TARGETS storm-conv-cli EXPORT storm_Targets RUNTIME DESTINATION bin LIBRARY DESTINATION lib OPTIONAL)
