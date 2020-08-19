set(storm_VERSION 1.6.1)

get_filename_component(storm_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

include("/Users/jipspel/Documents/Tools/storm/debug-build/resources/3rdparty/carl/carlConfig.cmake")


add_library(l3pp INTERFACE IMPORTED)
set_target_properties(l3pp PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "/Users/jipspel/Documents/Tools/storm/resources/3rdparty/l3pp/")

add_library(gmm INTERFACE IMPORTED)
set_target_properties(gmm PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "/Users/jipspel/Documents/Tools/storm/resources/3rdparty/gmm-5.2/include")

add_library(StormEigen INTERFACE IMPORTED)
set_target_properties(StormEigen PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "/Users/jipspel/Documents/Tools/storm/test/include/resources/3rdparty/StormEigen")

add_library(target-boost-1_SHARED SHARED IMPORTED)
set_target_properties(target-boost-1_SHARED PROPERTIES IMPORTED_LOCATION "/usr/local/lib/libboost_filesystem-mt.dylib")
set_target_properties(target-boost-1_SHARED PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "/usr/local/include")

add_library(target-boost-2_SHARED SHARED IMPORTED)
set_target_properties(target-boost-2_SHARED PROPERTIES IMPORTED_LOCATION "/usr/local/lib/libboost_system-mt.dylib")
set_target_properties(target-boost-2_SHARED PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "/usr/local/include")

add_library(ExprTk INTERFACE IMPORTED)
set_target_properties(ExprTk PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "/Users/jipspel/Documents/Tools/storm/resources/3rdparty/exprtk")

add_library(ModernJSON INTERFACE IMPORTED)
set_target_properties(ModernJSON PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "/Users/jipspel/Documents/Tools/storm/resources/3rdparty/modernjson/src/")

add_library(Z3_SHARED SHARED IMPORTED)
set_target_properties(Z3_SHARED PROPERTIES IMPORTED_LOCATION "/usr/local/lib/libz3.dylib")
set_target_properties(Z3_SHARED PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "/usr/local/include")

add_library(glpk_SHARED SHARED IMPORTED)
set_target_properties(glpk_SHARED PROPERTIES IMPORTED_LOCATION "/usr/local/lib/libglpk.dylib")
set_target_properties(glpk_SHARED PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "/usr/local/include")

add_library(cudd_STATIC STATIC IMPORTED)
set_target_properties(cudd_STATIC PROPERTIES IMPORTED_LOCATION "/Users/jipspel/Documents/Tools/storm/test/resources/3rdparty/cudd-3.0.0/lib/libcudd.a")
set_target_properties(cudd_STATIC PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "/Users/jipspel/Documents/Tools/storm/test/resources/3rdparty/cudd-3.0.0/include")

add_library(sylvan_STATIC STATIC IMPORTED)
set_target_properties(sylvan_STATIC PROPERTIES IMPORTED_LOCATION "/Users/jipspel/Documents/Tools/storm/test/resources/3rdparty/sylvan/src/libsylvan.a")
set_target_properties(sylvan_STATIC PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "/Users/jipspel/Documents/Tools/storm/resources/3rdparty/sylvan/src")

add_library(HWLOC_STATIC STATIC IMPORTED)
set_target_properties(HWLOC_STATIC PROPERTIES IMPORTED_LOCATION "/usr/local/lib/libhwloc.dylib")
set_target_properties(HWLOC_STATIC PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "")

add_library(CppTemplate INTERFACE IMPORTED)
set_target_properties(CppTemplate PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "/Users/jipspel/Documents/Tools/storm/resources/3rdparty/cpptemplate/")



set(STORM_USE_CLN_EA "OFF")
set(STORM_USE_CLN_RF "ON")
set(STORM_HAVE_XERCES "ON")

# Our library dependencies (contains definitions for IMPORTED targets)
if(NOT TARGET storm)
  include("${storm_CMAKE_DIR}/stormTargets.cmake")
endif()


####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was stormConfig.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################

set(storm_INCLUDE_DIR "${storm_CMAKE_DIR}/../../../include/storm")

set(storm_LIBRARIES storm)
check_required_components(storm)
