if(USE_XERCESC)
	find_package(XercesC QUIET)
    if(XercesC_FOUND)
        message(STATUS "Storm - Use system version of xerces.")
        set(STORM_HAVE_XERCES ON)
        include_directories(${XercesC_INCLUDE_DIRS})
        if(APPLE)
            FIND_LIBRARY(COREFOUNDATION_LIBRARY CoreFoundation )
            FIND_LIBRARY(CORESERVICES_LIBRARY CoreServices )
            mark_as_advanced(COREFOUNDATION_LIBRARY)
            mark_as_advanced(CORESERVICES_LIBRARY)
            string(REPLACE ".so" ".dylib" XercesC_LIBRARIES ${XercesC_LIBRARIES})

        endif()


        # find_package(CURL)
        message (STATUS "Storm (GSPN) - Linking with xercesc: ${XercesC_LIBRARIES}")

        list(APPEND STORM_GSPN_LINK_LIBRARIES ${XercesC_LIBRARIES} ${COREFOUNDATION_LIBRARY} ${CORESERVICES_LIBRARY} ${CURL_LIBRARIES})
    else()
        set(STORM_HAVE_XERCES OFF)
        message (STATUS "Storm - Building without Xerces disables parsing XML formats (for GSPNs)")
    endif()
endif(USE_XERCESC)

