set(SOURCES aiger.c)

set(HEADERS aiger.h)

option(BUILD_SHARED_LIBS "Enable/disable creation of shared libraries" ON)
option(BUILD_STATIC_LIBS "Enable/disable creation of static libraries" ON)

add_library(aiger ${SOURCES})

if(UNIX AND NOT APPLE)
    target_link_libraries(aiger rt)
endif()

install(TARGETS aiger DESTINATION "${CMAKE_INSTALL_LIBDIR}")
install(FILES ${HEADERS} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

# MODIFICATIONS NEEDED MADE FOR STORM

# We need to make sure that the binary is put into a folder that is independent of the
# build type. Otherwise -- for example when using Xcode -- the binary might end up in a
# sub-folder "Debug" or "Release".
set_target_properties(aiger PROPERTIES
                      ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR}
                      ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR})
