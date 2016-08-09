# Test Sources
# Note that the tests also need the source files, except for the main file
file(GLOB STORM_FUNCTIONAL_TEST_MAIN_FILE ${STORM_CPP_TESTS_BASE_PATH}/functional/storm-functional-tests.cpp)
file(GLOB_RECURSE STORM_FUNCTIONAL_TEST_FILES ${STORM_CPP_TESTS_BASE_PATH}/functional/*.h ${STORM_CPP_TESTS_BASE_PATH}/functional/*.cpp)

register_source_groups_from_filestructure("${STORM_FUNCTIONAL_TEST_FILES}")

add_executable(storm-functional-tests ${STORM_FUNCTIONAL_TEST_MAIN_FILE} ${STORM_FUNCTIONAL_TEST_FILES})
target_link_libraries(storm-functional-tests storm)
add_dependencies(storm-functional-tests test-resources)

include_directories(${GTEST_INCLUDE_DIR})
enable_testing()
target_link_libraries(storm-functional-tests ${STORM_TEST_LINK_LIBRARIES})
add_test(NAME storm-functional-tests COMMAND storm-functional-tests)

INSTALL(TARGETS storm-functional-tests
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)
