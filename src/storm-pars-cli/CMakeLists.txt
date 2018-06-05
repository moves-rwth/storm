# Create storm-pars.
add_executable(storm-pars-cli ${PROJECT_SOURCE_DIR}/src/storm-pars-cli/storm-pars.cpp)
target_link_libraries(storm-pars-cli storm-pars storm-cli-utilities) # Adding headers for xcode
set_target_properties(storm-pars-cli PROPERTIES OUTPUT_NAME "storm-pars")

add_dependencies(binaries storm-pars-cli)

# installation
install(TARGETS storm-pars-cli EXPORT storm_Targets RUNTIME DESTINATION bin LIBRARY DESTINATION lib OPTIONAL)
