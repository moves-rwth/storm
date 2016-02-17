add_custom_target(resources) 
add_custom_target(test-resources)


if(STORM_SUPPORT_XML_INPUT_FORMATS)
    ExternalProject_Add(
            xercesc
            SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/xercesc-3.1.2
            CONFIGURE_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/xercesc-3.1.2/configure --prefix=${CMAKE_CURRENT_BINARY_DIR}/xercesc-3.1.2 --libdir=${CMAKE_CURRENT_BINARY_DIR}/xercesc-3.1.2/lib CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER} CFLAGS=-O3 CXXFLAGS=-O3
            PREFIX ${CMAKE_CURRENT_BINARY_DIR}/xercesc-3.1.2
            BUILD_COMMAND make
            BUILD_IN_SOURCE 0
            LOG_CONFIGURE ON
            LOG_BUILD ON
            LOG_INSTALL ON
    )
    add_dependencies(resources xercesc)
endif()

ExternalProject_Add(
    glpk
    DOWNLOAD_COMMAND ""
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/glpk-4.57
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/glpk-4.57
    CONFIGURE_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/glpk-4.57/configure --prefix=${CMAKE_CURRENT_BINARY_DIR}/glpk-4.57  --libdir=${CMAKE_CURRENT_BINARY_DIR}/glpk-4.57/lib CC=${CMAKE_C_COMPILER}
    BUILD_COMMAND make "CFLAGS=-O2 -w"
    INSTALL_COMMAND make install
    BUILD_IN_SOURCE 0
    LOG_CONFIGURE ON
    LOG_BUILD ON
    LOG_INSTALL ON
)
add_dependencies(resources glpk)


ExternalProject_Add(
    cudd3
    DOWNLOAD_COMMAND ""
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cudd-3.0.0
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/cudd-3.0.0
    CONFIGURE_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/cudd-3.0.0/configure  --enable-shared   --enable-obj --prefix=${CMAKE_CURRENT_BINARY_DIR}/cudd-3.0.0 --libdir=${CMAKE_CURRENT_BINARY_DIR}/cudd-3.0.0/lib CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER}
    BUILD_COMMAND make "CFLAGS=-O2 -w"
    INSTALL_COMMAND make install
    BUILD_IN_SOURCE 0
)
add_dependencies(resources cudd3)

ExternalProject_Get_Property(cudd3 binary_dir)
set(CUDD3_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/cudd-3.0.0/include PARENT_SCOPE)

set(CUDD3_SHARED_LIBRARIES ${CMAKE_CURRENT_BINARY_DIR}/cudd-3.0.0/lib/libcudd${DYNAMIC_EXT} PARENT_SCOPE)
set(CUDD3_STATIC_LIBRARIES ${CMAKE_CURRENT_BINARY_DIR}/cudd-3.0.0/liblibcudd${STATIC_EXT} PARENT_SCOPE)

ExternalProject_Add(
        googletest
        #For downloads (may be useful later!)
        #SVN_REPOSITORY http://googletest.googlecode.com/svn/trunk/
        #TIMEOUT 10
        DOWNLOAD_COMMAND ""
        SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/gtest-1.7.0"
        # Force the same output paths for debug and release builds so that
        # we know in which place the binaries end up when using the Xcode generator
        CMAKE_ARGS  -Dgtest_force_shared_crt=ON -DCXX=${CMAKE_CXX_COMPILER} -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG:PATH=${CMAKE_CURRENT_BINARY_DIR}/gtest-1.7.0 -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE:PATH=${CMAKE_CURRENT_BINARY_DIR}/gtest-1.7.0 
        # Disable install step
        INSTALL_COMMAND ""
        BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/gtest-1.7.0"
        INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/gtest-1.7.0"
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
    
