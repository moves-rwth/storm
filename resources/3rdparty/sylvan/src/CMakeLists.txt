add_library(sylvan STATIC)
add_library(sylvan::sylvan ALIAS sylvan)

target_sources(sylvan
  PRIVATE
    sha2.c
    sylvan_bdd.c
    sylvan_cache.c
    sylvan_common.c
    sylvan_hash.c
    sylvan_ldd.c
    sylvan_mt.c
    sylvan_mtbdd.c
    sylvan_obj.cpp
    sylvan_refs.c
    sylvan_sl.c
    sylvan_stats.c
    sylvan_table.c
    sylvan_zdd.c
# Added by Storm
    lace.c
    storm_wrapper.cpp
    sylvan_bdd_storm.c
    sylvan_mtbdd_storm.c
    sylvan_obj_storm.cpp
    sylvan_storm_rational_function.c
    sylvan_storm_rational_number.c
  PUBLIC
  PUBLIC
    sylvan.h
    sylvan_bdd.h
    sylvan_cache.h
    sylvan_config.h
    sylvan_common.h
    sylvan_hash.h
    sylvan_int.h
    sylvan_ldd.h
    sylvan_ldd_int.h
    sylvan_mt.h
    sylvan_mtbdd.h
    sylvan_mtbdd_int.h
    sylvan_obj.hpp
    sylvan_stats.h
    sylvan_table.h
    sylvan_tls.h
    sylvan_zdd.h
    sylvan_zdd_int.h
# Added by Storm
    lace.h
    lace_config.h
    storm_wrapper.h
    sylvan_bdd_storm.h
    sylvan_mtbdd_storm.h
    sylvan_storm_rational_function.h
    sylvan_storm_rational_number.h
)

### STORM MODIFICATIONS FOR CARL
# C++11 -> C++14
# C++14 is needed for CARL
set_target_properties(sylvan PROPERTIES VERSION ${sylvan_VERSION} SOVERSION ${sylvan_VERSION_MAJOR})
target_compile_features(sylvan PUBLIC c_std_11 cxx_std_14)
target_compile_options(sylvan PRIVATE -Wall -Wextra -fno-strict-aliasing -Wno-deprecated)
target_include_directories(sylvan PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(sylvan PUBLIC m pthread)

### MODIFICATIONS NEEDED FOR STORM
target_include_directories(sylvan PUBLIC ${PROJECT_SOURCE_DIR}/../../../src)
target_include_directories(sylvan PUBLIC ${PROJECT_BINARY_DIR}/../../../include)


option(SYLVAN_GMP "Include custom MTBDD type GMP")
if(SYLVAN_GMP)
    # We only want to include the custom MTBDD type GMP if we actually have the GMP library
    find_package(GMP REQUIRED)
    target_sources(sylvan PRIVATE sylvan_gmp.c PUBLIC sylvan_gmp.h)
    target_include_directories(sylvan PRIVATE ${GMP_INCLUDE_DIR})
    target_link_libraries(sylvan PUBLIC ${GMP_LIBRARIES})
endif()

### STORM MODIFICATIONS FOR CARL

option(USE_CARL "Sets whether carl should be included." ON)
if(USE_CARL)
    target_compile_definitions(sylvan PRIVATE SYLVAN_HAVE_CARL)
    message(STATUS "Sylvan - Using CArL.")
    find_package(carl REQUIRED PATHS ${carl_DIR} NO_DEFAULT_PATH)
else()
    message(STATUS "Sylvan - Not using CArL.")
endif()

###


# Do we want to collect BDD statistics?
option(SYLVAN_STATS "Let Sylvan collect statistics at runtime" OFF)
if(SYLVAN_STATS)
    set_target_properties(sylvan PROPERTIES COMPILE_DEFINITIONS "SYLVAN_STATS")
endif()

# MODIFICATIONS NEEDED FOR STORM

set_target_properties(sylvan PROPERTIES
                      ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR}
                      ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR})

if(USE_CARL)
   message(STATUS "Sylvan - linking CArL.")
   target_link_libraries(sylvan PUBLIC ${carl_LIBRARIES})
endif(USE_CARL)
