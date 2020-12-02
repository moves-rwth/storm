set(SOURCES aiger.c)

set(HEADERS aiger.h)

option(BUILD_SHARED_LIBS "Enable/disable creation of shared libraries" ON)
option(BUILD_STATIC_LIBS "Enable/disable creation of static libraries" ON)

add_library(aiger ${SOURCES})
