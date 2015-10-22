include(ExternalProject)
ExternalProject_Add(
        xercesc
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/xercesc-3.1.2
        CONFIGURE_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/xercesc-3.1.2/configure --prefix=${CMAKE_CURRENT_BINARY_DIR}/xercesc-3.1.2
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/xercesc-3.1.2
        BUILD_COMMAND make
        BUILD_IN_SOURCE 1
)