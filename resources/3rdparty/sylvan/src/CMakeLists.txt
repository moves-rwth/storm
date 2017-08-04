set(SOURCES
    lace.c
    sha2.c
    sylvan_bdd.c
    sylvan_cache.c
    sylvan_common.c
    sylvan_gmp.c
    sylvan_ldd.c
    sylvan_mt.c
    sylvan_mtbdd.c
    sylvan_obj.cpp
    sylvan_refs.c
    sylvan_sl.c
    sylvan_stats.c
    sylvan_table.c
    storm_wrapper.cpp
    sylvan_storm_rational_function.c
    sylvan_storm_rational_number.c
)

set(HEADERS
    lace.h
    sylvan.h
    sylvan_bdd.h
    sylvan_cache.h
    sylvan_config.h
    sylvan_common.h
    sylvan_gmp.h
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
    storm_wrapper.h
    sylvan_bdd_storm.h
    sylvan_mtbdd_storm.h
    sylvan_storm_rational_function.h
    sylvan_storm_rational_number.h
)

option(BUILD_SHARED_LIBS "Enable/disable creation of shared libraries" ON)
option(BUILD_STATIC_LIBS "Enable/disable creation of static libraries" ON)

add_library(sylvan ${SOURCES})

find_package(GMP REQUIRED)

include_directories(sylvan ${GMP_INCLUDE_DIR})
target_link_libraries(sylvan m pthread ${GMP_LIBRARIES})

if(UNIX AND NOT APPLE)
    target_link_libraries(sylvan rt)
endif()

option(SYLVAN_STATS "Collect statistics" OFF)
if(SYLVAN_STATS)
    set_target_properties(sylvan PROPERTIES COMPILE_DEFINITIONS "SYLVAN_STATS")
endif()

install(TARGETS sylvan DESTINATION "${CMAKE_INSTALL_LIBDIR}")
install(FILES ${HEADERS} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

# MODIFICATIONS NEEDED MADE FOR STORM

# We need to make sure that the binary is put into a folder that is independent of the
# build type. Otherwise -- for example when using Xcode -- the binary might end up in a
# sub-folder "Debug" or "Release".
set_target_properties(sylvan PROPERTIES
                      ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR}
                      ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR})
                      
if(USE_CARL)
   message(STATUS "Sylvan - linking CArL.")
   target_link_libraries(sylvan ${carl_LIBRARIES})
endif(USE_CARL)