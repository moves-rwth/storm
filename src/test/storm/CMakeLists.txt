# Base path for test files
set(STORM_TESTS_BASE_PATH "${PROJECT_SOURCE_DIR}/src/test/storm")

# Test Sources
file(GLOB_RECURSE ALL_FILES ${STORM_TESTS_BASE_PATH}/*.h ${STORM_TESTS_BASE_PATH}/*.cpp)

register_source_groups_from_filestructure("${ALL_FILES}" test)

# Note that the tests also need the source files, except for the main file
include_directories(${GTEST_INCLUDE_DIR})

# Set split and non-split test directories
set(NON_SPLIT_TESTS abstraction adapter automata builder logic model parser simulator solver storage transformer utility)
set(MODELCHECKER_TEST_SPLITS abstraction csl exploration lexicographic multiobjective reachability)
set(MODELCHECKER_PRCTL_TEST_SPLITS dtmc mdp)

function(configure_testsuite_target testsuite)
	#message(CONFIGURING TESTSUITE '${testsuite}') #DEBUG
	target_link_libraries(test-${testsuite} storm storm-parsers storm-conv)
	target_link_libraries(test-${testsuite} ${STORM_TEST_LINK_LIBRARIES})

	add_dependencies(test-${testsuite} test-resources)
	add_test(NAME run-test-${testsuite} COMMAND $<TARGET_FILE:test-${testsuite}>)
	add_dependencies(tests test-${testsuite})
endfunction()

foreach(testsuite ${NON_SPLIT_TESTS})
	file(GLOB_RECURSE TEST_${testsuite}_FILES ${STORM_TESTS_BASE_PATH}/${testsuite}/*.h ${STORM_TESTS_BASE_PATH}/${testsuite}/*.cpp)
	add_executable(test-${testsuite} ${TEST_${testsuite}_FILES} ${STORM_TESTS_BASE_PATH}/storm-test.cpp)
	configure_testsuite_target(${testsuite})
endforeach()

# Modelchecker testsuite split
foreach(modelchecker_split ${MODELCHECKER_TEST_SPLITS})
	file(GLOB_RECURSE TEST_MODELCHECKER_${modelchecker_split}_FILES ${STORM_TESTS_BASE_PATH}/modelchecker/${modelchecker_split}/*.h ${STORM_TESTS_BASE_PATH}/modelchecker/${modelchecker_split}/*.cpp)
	add_executable(test-modelchecker-${modelchecker_split} ${TEST_MODELCHECKER_${modelchecker_split}_FILES} ${STORM_TESTS_BASE_PATH}/storm-test.cpp)
	configure_testsuite_target(modelchecker-${modelchecker_split})
endforeach()

# Modelchecker-Prctl testsuite split
foreach(prctl_split ${MODELCHECKER_PRCTL_TEST_SPLITS})
	file(GLOB_RECURSE TEST_MODELCHECKER_PRCTL_${prctl_split}_FILES ${STORM_TESTS_BASE_PATH}/modelchecker/prctl/${prctl_split}/*.h ${STORM_TESTS_BASE_PATH}/modelchecker/prctl/${prctl_split}/*.cpp)
	add_executable(test-modelchecker-prctl-${prctl_split} ${TEST_MODELCHECKER_PRCTL_${prctl_split}_FILES} ${STORM_TESTS_BASE_PATH}/storm-test.cpp)
	configure_testsuite_target(modelchecker-prctl-${prctl_split})
endforeach()
