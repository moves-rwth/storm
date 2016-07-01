# Test Sources
# Note that the tests also need the source files, except for the main file
file(GLOB STORM_FUNCTIONAL_TEST_MAIN_FILE ${STORM_CPP_TESTS_BASE_PATH}/functional/storm-functional-tests.cpp)
file(GLOB_RECURSE STORM_FUNCTIONAL_TEST_FILES ${STORM_CPP_TESTS_BASE_PATH}/functional/*.h ${STORM_CPP_TESTS_BASE_PATH}/functional/*.cpp)
file(GLOB STORM_PERFORMANCE_TEST_MAIN_FILE ${STORM_CPP_TESTS_BASE_PATH}/performance/storm-performance-tests.cpp)
file(GLOB_RECURSE STORM_PERFORMANCE_TEST_FILES ${STORM_CPP_TESTS_BASE_PATH}/performance/*.h ${STORM_CPP_TESTS_BASE_PATH}/performance/*.cpp)

source_group(functional-test FILES ${STORM_FUNCTIONAL_TEST_FILES})
source_group(performance-test FILES ${STORM_PERFORMANCE_TEST_FILES} ${})

add_executable(storm-functional-tests ${STORM_FUNCTIONAL_TEST_MAIN_FILE} ${STORM_FUNCTIONAL_TEST_FILES})
target_link_libraries(storm-functional-tests storm)
add_executable(storm-performance-tests ${STORM_PERFORMANCE_TEST_MAIN_FILE} ${STORM_PERFORMANCE_TEST_FILES})
target_link_libraries(storm-performance-tests storm)
add_dependencies(storm-functional-tests test-resources)
add_dependencies(storm-performance-tests test-resources)

include_directories(${GTEST_INCLUDE_DIR})
enable_testing()
target_link_libraries(storm-functional-tests ${STORM_TEST_LINK_LIBRARIES})
target_link_libraries(storm-performance-tests ${STORM_TEST_LINK_LIBRARIES})
add_test(NAME storm-functional-tests COMMAND storm-functional-tests)
add_test(NAME storm-performance-tests COMMAND storm-performance-tests)

INSTALL(TARGETS storm-functional-tests storm-performance-tests
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)
