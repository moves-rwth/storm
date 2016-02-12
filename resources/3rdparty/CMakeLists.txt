include(ExternalProject)
ExternalProject_Add(
        xercesc
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/xercesc-3.1.2
        CONFIGURE_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/xercesc-3.1.2/configure --prefix=${CMAKE_CURRENT_BINARY_DIR}/xercesc-3.1.2
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/xercesc-3.1.2
        BUILD_COMMAND make
        BUILD_IN_SOURCE 0
)

ExternalProject_Add(
    glpk
    DOWNLOAD_COMMAND ""
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/glpk-4.57
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/glpk-4.57
    CONFIGURE_COMMAND CC=${CMAKE_C_COMPILER} ${CMAKE_CURRENT_SOURCE_DIR}/glpk-4.57/configure --prefix=${CMAKE_CURRENT_BINARY_DIR}/glpk-4.57
    BUILD_COMMAND make
    INSTALL_COMMAND make install
    BUILD_IN_SOURCE 0
)
