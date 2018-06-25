# Create storm-dft.
add_executable(storm-dft-cli ${PROJECT_SOURCE_DIR}/src/storm-dft-cli/storm-dft.cpp)
target_link_libraries(storm-dft-cli storm-dft storm-cli-utilities) # Adding headers for xcode
set_target_properties(storm-dft-cli PROPERTIES OUTPUT_NAME "storm-dft")

add_dependencies(binaries storm-dft-cli)

# installation
install(TARGETS storm-dft-cli EXPORT storm_Targets RUNTIME DESTINATION bin LIBRARY DESTINATION lib OPTIONAL)
