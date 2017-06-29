# Create storm-pars.
add_executable(storm-pars-cli ${PROJECT_SOURCE_DIR}/src/storm-pars-cli/storm-pars.cpp)
target_link_libraries(storm-pars-cli storm-pars) # Adding headers for xcode
set_target_properties(storm-pars-cli PROPERTIES OUTPUT_NAME "storm-pars")

# installation
install(TARGETS storm-pars-cli RUNTIME DESTINATION bin LIBRARY DESTINATION lib OPTIONAL)
