# Base path for test files
set(STORM_TESTS_BASE_PATH "${PROJECT_SOURCE_DIR}/src/test/storm-permissive")

# Test Sources
file(GLOB_RECURSE ALL_FILES ${STORM_TESTS_BASE_PATH}/*.h ${STORM_TESTS_BASE_PATH}/*.cpp)

register_source_groups_from_filestructure("${ALL_FILES}" test)

# Note that the tests also need the source files, except for the main file
include_directories(${GTEST_INCLUDE_DIR})

foreach (testsuite analysis)
    file(GLOB_RECURSE TEST_${testsuite}_FILES ${STORM_TESTS_BASE_PATH}/${testsuite}/*.h ${STORM_TESTS_BASE_PATH}/${testsuite}/*.cpp)
    add_executable(test-permissive-${testsuite} ${TEST_${testsuite}_FILES} ${STORM_TESTS_BASE_PATH}/storm-test.cpp)
    target_link_libraries(test-permissive-${testsuite} storm-permissive storm-parsers)
    target_link_libraries(test-permissive-${testsuite} ${STORM_TEST_LINK_LIBRARIES})

    add_dependencies(test-permissive-${testsuite} test-resources)
    add_test(NAME run-test-permissive-${testsuite} COMMAND $<TARGET_FILE:test-permissive-${testsuite}>)
    add_dependencies(tests test-permissive-${testsuite})

endforeach ()
