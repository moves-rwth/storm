add_custom_target(resources) 
add_custom_target(test-resources)

include(ExternalProject)
ExternalProject_Add(
        xercesc
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/xercesc-3.1.2
        CONFIGURE_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/xercesc-3.1.2/configure --prefix=${CMAKE_CURRENT_BINARY_DIR}/xercesc-3.1.2 CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER} CFLAGS=-O3 CXXFLAGS=-O3
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/xercesc-3.1.2
        BUILD_COMMAND make
        BUILD_IN_SOURCE 0
        LOG_CONFIGURE ON
        LOG_BUILD ON
        LOG_INSTALL ON
)
add_dependencies(resources xercesc)

ExternalProject_Add(
    glpk
    DOWNLOAD_COMMAND ""
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/glpk-4.57
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/glpk-4.57
    CONFIGURE_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/glpk-4.57/configure --prefix=${CMAKE_CURRENT_BINARY_DIR}/glpk-4.57 CC=${CMAKE_C_COMPILER}
    BUILD_COMMAND make "CFLAGS=-O2 -w"
    INSTALL_COMMAND make install
    BUILD_IN_SOURCE 0
    LOG_CONFIGURE ON
    LOG_BUILD ON
    LOG_INSTALL ON
)
add_dependencies(resources glpk)


ExternalProject_Add(
        googletest
        #For downloads (may be useful later!)
        #SVN_REPOSITORY http://googletest.googlecode.com/svn/trunk/
        #TIMEOUT 10
        DOWNLOAD_COMMAND ""
        SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/gtest-1.7.0"
        # Force separate output paths for debug and release builds to allow easy
        # identification of correct lib in subsequent TARGET_LINK_LIBRARIES 
        CMAKE_ARGS  -Dgtest_force_shared_crt=ON
        # Disable install step
        INSTALL_COMMAND ""
        # Wrap download, configure and build steps in a script to log output
        LOG_CONFIGURE ON
        LOG_BUILD ON)

# Specify include dir
ExternalProject_Get_Property(googletest source_dir)
set(GTEST_INCLUDE_DIR ${source_dir}/include PARENT_SCOPE)
# Specify MainTest's link libraries
ExternalProject_Get_Property(googletest binary_dir)
set(GTEST_LIBRARIES ${binary_dir}/libgtest.a ${binary_dir}/libgtest_main.a PARENT_SCOPE)

add_dependencies(test-resources googletest)
    
