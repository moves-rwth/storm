if(USE_XERCES)
    find_package(Xerces QUIET REQUIRED)
    if(XERCES_FOUND)
        message(STATUS "Storm - Use system version of xerces.")
    else()
        message(STATUS "Storm - Use shipped version of xerces.")
        set(XERCES_LIB_DIR ${STORM_3RDPARTY_BINARY_DIR}/xercesc-3.1.2/lib)
        ExternalProject_Add(
                xercesc
                SOURCE_DIR ${STORM_3RDPARTY_SOURCE_DIR}/xercesc-3.1.2
                CONFIGURE_COMMAND ${STORM_3RDPARTY_SOURCE_DIR}/xercesc-3.1.2/configure --prefix=${STORM_3RDPARTY_BINARY_DIR}/xercesc-3.1.2 --libdir=${XERCES_LIB_DIR} CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER} CFLAGS=-O3 CXXFLAGS=-O3
                PREFIX ${STORM_3RDPARTY_BINARY_DIR}/xercesc-3.1.2
                BUILD_COMMAND make
                BUILD_IN_SOURCE 0
                LOG_CONFIGURE ON
                LOG_BUILD ON
                LOG_INSTALL ON
                BUILD_BYPRODUCTS ${XERCES_LIB_DIR}/libxerces-c${DYNAMIC_EXT} ${XERCES_LIB_DIR}/libxerces-c${STATIC_EXT}
        )

        set(XERCES_ROOT ${STORM_3RDPARTY_BINARY_DIR}/xercesc-3.1.2)
        set(XERCESC_INCLUDE ${XERCES_ROOT}/include)
        set(XERCES_LIBRARY_PATH ${XERCES_ROOT}/lib)

        if(BUILD_STATIC)
            set(XERCESC_LIBRARIES ${XERCES_LIBRARY_PATH}/libxerces-c${STATIC_EXT})
        else()
            set(XERCESC_LIBRARIES ${XERCES_LIBRARY_PATH}/libxerces-c${DYNAMIC_EXT})
        endif()

        add_dependencies(resources xercesc)
    endif()

    message (STATUS "Storm - Linking with xercesc.")
    set(STORM_HAVE_XERCES ON)
    include_directories(${XERCESC_INCLUDE})
    if(APPLE)
        FIND_LIBRARY(COREFOUNDATION_LIBRARY CoreFoundation )
        FIND_LIBRARY(CORESERVICES_LIBRARY CoreServices )
	mark_as_advanced(COREFOUNDATION_LIBRARY)
	mark_as_advanced(CORESERVICES_LIBRARY)
    endif()
    find_package(CURL)
    list(APPEND STORM_LINK_LIBRARIES ${XERCESC_LIBRARIES} ${COREFOUNDATION_LIBRARY} ${CORESERVICES_LIBRARY} ${CURL_LIBRARIES})
else()
    message (WARNING "StoRM - Building without Xerces disables parsing XML formats (for GSPNs)")
endif(USE_XERCES)
