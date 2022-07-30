target_sources(sylvan
  PRIVATE
    lace.c
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
    storm_wrapper.cpp
    sylvan_bdd_storm.c
    sylvan_mtbdd_storm.c
    sylvan_obj_storm.cpp
    sylvan_storm_rational_function.c
    sylvan_storm_rational_number.c
  PUBLIC
    $<INSTALL_INTERFACE:include/lace.h>
    $<INSTALL_INTERFACE:include/sylvan.h>
    $<INSTALL_INTERFACE:include/sylvan_bdd.h>
    $<INSTALL_INTERFACE:include/sylvan_cache.h>
    $<INSTALL_INTERFACE:include/sylvan_config.h>
    $<INSTALL_INTERFACE:include/sylvan_common.h>
    $<INSTALL_INTERFACE:include/sylvan_hash.h>
    $<INSTALL_INTERFACE:include/sylvan_int.h>
    $<INSTALL_INTERFACE:include/sylvan_ldd.h>
    $<INSTALL_INTERFACE:include/sylvan_ldd_int.h>
    $<INSTALL_INTERFACE:include/sylvan_mt.h>
    $<INSTALL_INTERFACE:include/sylvan_mtbdd.h>
    $<INSTALL_INTERFACE:include/sylvan_mtbdd_int.h>
    $<INSTALL_INTERFACE:include/sylvan_obj.hpp>
    $<INSTALL_INTERFACE:include/sylvan_stats.h>
    $<INSTALL_INTERFACE:include/sylvan_table.h>
    $<INSTALL_INTERFACE:include/sylvan_tls.h>
    $<INSTALL_INTERFACE:include/sylvan_zdd.h>
    $<INSTALL_INTERFACE:include/sylvan_zdd_int.h>
    $<INSTALL_INTERFACE:include/storm_wrapper.h>
    $<INSTALL_INTERFACE:include/sylvan_bdd_storm.h>
    $<INSTALL_INTERFACE:include/sylvan_mtbdd_storm.h>
    $<INSTALL_INTERFACE:include/sylvan_storm_rational_function.h>
    $<INSTALL_INTERFACE:include/sylvan_storm_rational_number.h>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/lace.h>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/sylvan.h>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/sylvan_bdd.h>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/sylvan_cache.h>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/sylvan_config.h>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/sylvan_common.h>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/sylvan_hash.h>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/sylvan_int.h>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/sylvan_ldd.h>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/sylvan_ldd_int.h>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/sylvan_mt.h>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/sylvan_mtbdd.h>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/sylvan_mtbdd_int.h>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/sylvan_obj.hpp>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/sylvan_stats.h>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/sylvan_table.h>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/sylvan_tls.h>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/sylvan_zdd.h>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/sylvan_zdd_int.h>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/storm_wrapper.h>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/sylvan_bdd_storm.h>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/sylvan_mtbdd_storm.h>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/sylvan_storm_rational_function.h>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/sylvan_storm_rational_number.h>
)

### STORM MDOIFICATIONS FOR CARL
# C++11 -> C++14
# C++14 is needed for CARL
target_compile_features(sylvan PUBLIC c_std_11 cxx_std_14)
target_compile_options(sylvan PRIVATE -Wall -Wextra -fno-strict-aliasing -Wno-deprecated)

target_include_directories(sylvan
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../../src>
    $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/../../../include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(sylvan PUBLIC m pthread)


option(SYLVAN_GMP "Include custom MTBDD type GMP")
if(SYLVAN_GMP)
    # We only want to include the custom MTBDD type GMP if we actually have the GMP library
    find_package(GMP REQUIRED)
    target_sources(sylvan
            PRIVATE
            sylvan_gmp.c
            PUBLIC
            $<INSTALL_INTERFACE:include/sylvan_gmp.h>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/sylvan_gmp.h>
            )
    target_include_directories(sylvan PRIVATE ${GMP_INCLUDE_DIR})
    target_link_libraries(sylvan PUBLIC ${GMP_LIBRARIES})
endif()

### STORM MDOIFICATIONS FOR CARL

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
option(SYLVAN_STATS "Collect statistics" OFF)
if(SYLVAN_STATS)
  set_target_properties(sylvan PROPERTIES COMPILE_DEFINITIONS "SYLVAN_STATS")
endif()


get_target_property(sylvan_PUBLIC_HEADERS sylvan INTERFACE_SOURCES)
set_target_properties(sylvan PROPERTIES PUBLIC_HEADER "${sylvan_PUBLIC_HEADERS}")


if(NOT subproject)
  include(GNUInstallDirs)
  include(CMakePackageConfigHelpers)

  install(
    TARGETS sylvan
    EXPORT sylvan-export
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
  )
  install(
    EXPORT sylvan-export
    FILE sylvanTargets.cmake
    NAMESPACE sylvan::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sylvan
  )
  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/sylvan.pc.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/sylvan.pc" @ONLY)
  install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/sylvan.pc"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig"
  )
endif()

# MODIFICATIONS NEEDED FOR STORM

set_target_properties(sylvan PROPERTIES
                      ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR}
                      ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR})

if(USE_CARL)
   message(STATUS "Sylvan - linking CArL.")
   target_link_libraries(sylvan PUBLIC ${carl_LIBRARIES})
endif(USE_CARL)
