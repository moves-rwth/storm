# Base path for test files
set(STORM_TESTS_BASE_PATH "${PROJECT_SOURCE_DIR}/src/test/storm-figaro")

# Test Sources
file(GLOB_RECURSE ALL_FILES ${STORM_TESTS_BASE_PATH}/*.h ${STORM_TESTS_BASE_PATH}/*.cpp)

register_source_groups_from_filestructure("${ALL_FILES}" test)

# Note that the tests also need the source files, except for the main file
include_directories(${GTEST_INCLUDE_DIR})

foreach (testsuite api)

    file(GLOB_RECURSE TEST_${testsuite}_FILES ${STORM_TESTS_BASE_PATH}/${testsuite}/*.h ${STORM_TESTS_BASE_PATH}/${testsuite}/*.cpp)
    add_executable (test-figaro-${testsuite} ${TEST_${testsuite}_FILES} ${STORM_TESTS_BASE_PATH}/storm-test.cpp api/figaro-test-headers.h api/FigaroApproximationTest.cpp)
    target_link_libraries(test-figaro-${testsuite} storm-figaro storm-parsers)
    target_link_libraries(test-figaro-${testsuite} ${STORM_TEST_LINK_LIBRARIES})

    add_dependencies(test-figaro-${testsuite} test-resources)
    add_test(NAME run-test-figaro-${testsuite} COMMAND $<TARGET_FILE:test-figaro-${testsuite}>)
    add_dependencies(tests test-figaro-${testsuite})

endforeach ()
