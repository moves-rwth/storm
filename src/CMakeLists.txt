
#############################################################
##
##	Source file aggregation and clustering
##
#############################################################
file(GLOB_RECURSE STORM_HEADERS ${PROJECT_SOURCE_DIR}/src/*.h)
file(GLOB_RECURSE STORM_HEADERS_CLI ${PROJECT_SOURCE_DIR}/src/cli/*.h)
file(GLOB_RECURSE STORM_SOURCES_WITHOUT_MAIN ${PROJECT_SOURCE_DIR}/src/*/*.cpp)
file(GLOB_RECURSE STORM_SOURCES_CLI ${PROJECT_SOURCE_DIR}/src/cli/*.cpp)
file(GLOB_RECURSE STORM_MAIN_FILE ${PROJECT_SOURCE_DIR}/src/storm.cpp)
file(GLOB_RECURSE STORM_GSPN_FILE ${PROJECT_SOURCE_DIR}/src/storm-gspn.cpp)
file(GLOB_RECURSE STORM_ADAPTERS_FILES ${PROJECT_SOURCE_DIR}/src/adapters/*.h ${PROJECT_SOURCE_DIR}/src/adapters/*.cpp)
file(GLOB_RECURSE STORM_BUILDER_FILES ${PROJECT_SOURCE_DIR}/src/builder/*.h ${PROJECT_SOURCE_DIR}/src/builder/*.cpp)
file(GLOB_RECURSE STORM_CLI_FILES ${PROJECT_SOURCE_DIR}/src/cli/*.h ${PROJECT_SOURCE_DIR}/src/cli/*.cpp)
file(GLOB_RECURSE STORM_EXCEPTIONS_FILES ${PROJECT_SOURCE_DIR}/src/exceptions/*.h ${PROJECT_SOURCE_DIR}/src/exceptions/*.cpp)
file(GLOB_RECURSE STORM_LOGIC_FILES ${PROJECT_SOURCE_DIR}/src/logic/*.h ${PROJECT_SOURCE_DIR}/src/logic/*.cpp)
file(GLOB STORM_MODELCHECKER_FILES ${PROJECT_SOURCE_DIR}/src/modelchecker/*.h ${PROJECT_SOURCE_DIR}/src/modelchecker/*.cpp)
file(GLOB STORM_MODELCHECKER_PRCTL_FILES ${PROJECT_SOURCE_DIR}/src/modelchecker/prctl/*.h ${PROJECT_SOURCE_DIR}/src/modelchecker/prctl/*.cpp)
file(GLOB_RECURSE STORM_MODELCHECKER_PRCTL_HELPER_FILES ${PROJECT_SOURCE_DIR}/src/modelchecker/prctl/helper/*.h ${PROJECT_SOURCE_DIR}/src/modelchecker/prctl/helper/*.cpp)
file(GLOB STORM_MODELCHECKER_CSL_FILES ${PROJECT_SOURCE_DIR}/src/modelchecker/csl/*.h ${PROJECT_SOURCE_DIR}/src/modelchecker/csl/*.cpp)
file(GLOB_RECURSE STORM_MODELCHECKER_CSL_HELPER_FILES ${PROJECT_SOURCE_DIR}/src/modelchecker/csl/helper/*.h ${PROJECT_SOURCE_DIR}/src/modelchecker/csl/helper/*.cpp)
file(GLOB_RECURSE STORM_MODELCHECKER_REACHABILITY_FILES ${PROJECT_SOURCE_DIR}/src/modelchecker/reachability/*.h ${PROJECT_SOURCE_DIR}/src/modelchecker/reachability/*.cpp)
file(GLOB_RECURSE STORM_MODELCHECKER_PROPOSITIONAL_FILES ${PROJECT_SOURCE_DIR}/src/modelchecker/propositional/*.h ${PROJECT_SOURCE_DIR}/src/modelchecker/propositional/*.cpp)
file(GLOB_RECURSE STORM_MODELCHECKER_RESULTS_FILES ${PROJECT_SOURCE_DIR}/src/modelchecker/results/*.h ${PROJECT_SOURCE_DIR}/src/modelchecker/results/*.cpp)
file(GLOB_RECURSE STORM_COUNTEREXAMPLES_FILES ${PROJECT_SOURCE_DIR}/src/counterexamples/*.h ${PROJECT_SOURCE_DIR}/src/counterexamples/*.cpp)
file(GLOB_RECURSE STORM_PERMISSIVESCHEDULER_FILES ${PROJECT_SOURCE_DIR}/src/permissivesched/*.h ${PROJECT_SOURCE_DIR}/src/permissivesched/*.cpp)
file(GLOB STORM_MODELS_FILES ${PROJECT_SOURCE_DIR}/src/models/*.h ${PROJECT_SOURCE_DIR}/src/models/*.cpp)
file(GLOB_RECURSE STORM_MODELS_SPARSE_FILES ${PROJECT_SOURCE_DIR}/src/models/sparse/*.h ${PROJECT_SOURCE_DIR}/src/models/sparse/*.cpp)
file(GLOB_RECURSE STORM_MODELS_SYMBOLIC_FILES ${PROJECT_SOURCE_DIR}/src/models/symbolic/*.h ${PROJECT_SOURCE_DIR}/src/models/symbolic/*.cpp)
file(GLOB STORM_PARSER_FILES ${PROJECT_SOURCE_DIR}/src/parser/*.h ${PROJECT_SOURCE_DIR}/src/parser/*.cpp)
file(GLOB_RECURSE STORM_PARSER_PRISMPARSER_FILES ${PROJECT_SOURCE_DIR}/src/parser/prismparser/*.h ${PROJECT_SOURCE_DIR}/src/parser/prismparser/*.cpp)
file(GLOB STORM_SETTINGS_FILES ${PROJECT_SOURCE_DIR}/src/settings/*.h ${PROJECT_SOURCE_DIR}/src/settings/*.cpp)
file(GLOB STORM_SETTINGS_MODULES_FILES ${PROJECT_SOURCE_DIR}/src/settings/modules/*.h ${PROJECT_SOURCE_DIR}/src/settings/modules/*.cpp)
file(GLOB_RECURSE STORM_SOLVER_FILES ${PROJECT_SOURCE_DIR}/src/solver/*.h ${PROJECT_SOURCE_DIR}/src/solver/*.cpp)
file(GLOB STORM_STORAGE_FILES ${PROJECT_SOURCE_DIR}/src/storage/*.h ${PROJECT_SOURCE_DIR}/src/storage/*.cpp)
file(GLOB_RECURSE STORM_STORAGE_DD_FILES ${PROJECT_SOURCE_DIR}/src/storage/dd/*.h ${PROJECT_SOURCE_DIR}/src/storage/dd/*.cpp)
file(GLOB_RECURSE STORM_STORAGE_EXPRESSIONS_FILES ${PROJECT_SOURCE_DIR}/src/storage/expressions/*.h ${PROJECT_SOURCE_DIR}/src/storage/expressions/*.cpp)
file(GLOB_RECURSE STORM_STORAGE_PRISM_FILES ${PROJECT_SOURCE_DIR}/src/storage/prism/*.h ${PROJECT_SOURCE_DIR}/src/storage/prism/*.cpp)
file(GLOB_RECURSE STORM_STORAGE_SPARSE_FILES ${PROJECT_SOURCE_DIR}/src/storage/sparse/*.h ${PROJECT_SOURCE_DIR}/src/storage/sparse/*.cpp)
file(GLOB_RECURSE STORM_UTILITY_FILES ${PROJECT_SOURCE_DIR}/src/utility/*.h ${PROJECT_SOURCE_DIR}/src/utility/*.cpp)


# Additional include files like the storm-config.h
file(GLOB_RECURSE STORM_BUILD_HEADERS ${PROJECT_BINARY_DIR}/include/*.h)

set(STORM_LIB_SOURCES ${STORM_SOURCES_WITHOUT_MAIN})
list(REMOVE_ITEM STORM_LIB_SOURCES ${STORM_SOURCES_CLI})
set(STORM_LIB_HEADERS ${STORM_HEADERS})
list(REMOVE_ITEM STORM_LIB_HEADERS ${STORM_HEADERS_CLI})
set(STORM_MAIN_SOURCES ${STORM_SOURCES_CLI} ${STORM_MAIN_FILE})
set(STORM_MAIN_HEADERS ${STORM_HEADERS_CLI})

# Group the headers and sources
source_group(main FILES ${STORM_MAIN_FILE})
source_group(adapters FILES ${STORM_ADAPTERS_FILES})
source_group(builder FILES ${STORM_BUILDER_FILES})
source_group(cli FILES ${STORM_CLI_FILES})
source_group(exceptions FILES ${STORM_EXCEPTIONS_FILES})
source_group(logic FILES ${STORM_LOGIC_FILES})
source_group(generated FILES ${STORM_BUILD_HEADERS} ${STORM_BUILD_SOURCES})
source_group(modelchecker FILES ${STORM_MODELCHECKER_FILES})
source_group(modelchecker\\prctl FILES ${STORM_MODELCHECKER_PRCTL_FILES})
source_group(modelchecker\\prctl\\helper FILES ${STORM_MODELCHECKER_PRCTL_HELPER_FILES})
source_group(modelchecker\\csl FILES ${STORM_MODELCHECKER_CSL_FILES})
source_group(modelchecker\\csl\\helper FILES ${STORM_MODELCHECKER_CSL_HELPER_FILES})
source_group(modelchecker\\reachability FILES ${STORM_MODELCHECKER_REACHABILITY_FILES})
source_group(modelchecker\\propositional FILES ${STORM_MODELCHECKER_PROPOSITIONAL_FILES})
source_group(modelchecker\\results FILES ${STORM_MODELCHECKER_RESULTS_FILES})
source_group(counterexamples FILES ${STORM_COUNTEREXAMPLES_FILES})
source_group(permissiveschedulers FILES ${STORM_PERMISSIVESCHEDULER_FILES})
source_group(models FILES ${STORM_MODELS_FILES})
source_group(models\\sparse FILES ${STORM_MODELS_SPARSE_FILES})
source_group(models\\symbolic FILES ${STORM_MODELS_SYMBOLIC_FILES})
source_group(parser FILES ${STORM_PARSER_FILES})
source_group(parser\\prismparser FILES ${STORM_PARSER_PRISMPARSER_FILES})
source_group(settings FILES ${STORM_SETTINGS_FILES})
source_group(settings\\modules FILES ${STORM_SETTINGS_MODULES_FILES})
source_group(solver FILES ${STORM_SOLVER_FILES})
source_group(storage FILES ${STORM_STORAGE_FILES})
source_group(storage\\dd FILES ${STORM_STORAGE_DD_FILES})
source_group(storage\\expressions FILES ${STORM_STORAGE_EXPRESSIONS_FILES})
source_group(storage\\prism FILES ${STORM_STORAGE_PRISM_FILES})
source_group(storage\\sparse FILES ${STORM_STORAGE_SPARSE_FILES})
source_group(utility FILES ${STORM_UTILITY_FILES})

# Add custom additional include or link directories
if (ADDITIONAL_INCLUDE_DIRS)
	message(STATUS "StoRM - Using additional include directories ${ADDITIONAL_INCLUDE_DIRS}")
	include_directories(${ADDITIONAL_INCLUDE_DIRS})
endif(ADDITIONAL_INCLUDE_DIRS)
if (ADDITIONAL_LINK_DIRS)
	message(STATUS "StoRM - Using additional link directories ${ADDITIONAL_LINK_DIRS}")
	link_directories(${ADDITIONAL_LINK_DIRS})
endif(ADDITIONAL_LINK_DIRS)

###############################################################################
##                                                                            #
##	Executable Creation                                                       #
##                                                                            #
##  All link_directories() calls MUST be made before this point               #
##                                                                            #
###############################################################################
add_library(storm ${STORM_LIB_SOURCES} ${STORM_LIB_HEADERS} ${STORM_GENERATED_SOURCES}) # Adding headers for xcode
add_executable(storm-main ${STORM_MAIN_SOURCES} ${STORM_MAIN_HEADERS})
target_link_libraries(storm-main storm) # Adding headers for xcode
set_target_properties(storm-main PROPERTIES OUTPUT_NAME "storm")
add_executable(storm-gspn ${STORM_GSPN_FILE} ${STORM_SOURCES_CLI} ${STORM_MAIN_HEADERS})
target_link_libraries(storm-gspn storm) # Adding headers for xcode
set_target_properties(storm-gspn PROPERTIES OUTPUT_NAME "gspn")

target_link_libraries(storm ${STORM_LINK_LIBRARIES})



INSTALL(TARGETS storm-main 
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)
