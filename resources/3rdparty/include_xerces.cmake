if(USE_XERCESC)
	find_package(XercesC QUIET)
    if(XercesC_FOUND)
        message(STATUS "Storm - Use system version of xerces.")
    else()
        message(STATUS "Storm - Use shipped version of xerces.")
        set(XercesC_LIB_DIR ${STORM_3RDPARTY_BINARY_DIR}/xercesc-3.1.2/lib)
        ExternalProject_Add(
                xercesc
                SOURCE_DIR ${STORM_3RDPARTY_SOURCE_DIR}/xercesc-3.1.2
                CONFIGURE_COMMAND ${STORM_3RDPARTY_SOURCE_DIR}/xercesc-3.1.2/configure --prefix=${STORM_3RDPARTY_BINARY_DIR}/xercesc-3.1.2 --libdir=${XercesC_LIB_DIR} CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER} CFLAGS=-O3 CXXFLAGS=-O3
                PREFIX ${STORM_3RDPARTY_BINARY_DIR}/xercesc-3.1.2
                BUILD_COMMAND make
                BUILD_IN_SOURCE 0
                LOG_CONFIGURE ON
                LOG_BUILD ON
                LOG_INSTALL ON
                BUILD_BYPRODUCTS ${XercesC_LIB_DIR}/libxerces-c${DYNAMIC_EXT} ${XercesC_LIB_DIR}/libxerces-c${STATIC_EXT}
        )

        set(XercesC_ROOT ${STORM_3RDPARTY_BINARY_DIR}/xercesc-3.1.2)
        set(XercesC_INCLUDE_DIRS ${XercesC_ROOT}/include)
        set(XercesC_LIBRARY_PATH ${XercesC_LIB_DIR})

        if(BUILD_STATIC)
            set(XercesC_LIBRARIES ${XercesC_LIBRARY_PATH}/libxerces-c${STATIC_EXT})
        else()
            set(XercesC_LIBRARIES ${XercesC_LIBRARY_PATH}/libxerces-c${DYNAMIC_EXT})
        endif()

        add_dependencies(resources xercesc)
    endif()

    message (STATUS "Storm - Linking with xercesc.")
    set(STORM_HAVE_XERCES ON)
    include_directories(${XercesC_INCLUDE_DIRS})
    if(APPLE)
        FIND_LIBRARY(COREFOUNDATION_LIBRARY CoreFoundation )
        FIND_LIBRARY(CORESERVICES_LIBRARY CoreServices )
		mark_as_advanced(COREFOUNDATION_LIBRARY)
		mark_as_advanced(CORESERVICES_LIBRARY)
    endif()
    # find_package(CURL)
    list(APPEND STORM_GSPN_LINK_LIBRARIES ${XercesC_LIBRARIES} ${COREFOUNDATION_LIBRARY} ${CORESERVICES_LIBRARY} ${CURL_LIBRARIES})
else()
    set(STORM_HAVE_XERCES OFF)
    message (WARNING "Storm - Building without Xerces disables parsing XML formats (for GSPNs)")
endif(USE_XERCESC)
