add_imported_library(GTESTCORE STATIC "@googletest@/lib/${CMAKE_FIND_LIBRARY_PREFIXES}gtest${STATIC_EXT}" "@googletest@/include")
add_imported_library(GTESTMAIN STATIC "@googletest@/lib/${CMAKE_FIND_LIBRARY_PREFIXES}gtest_main${STATIC_EXT}" "@googletest@/include")
set(GTEST_LIBRARIES GTESTCORE_STATIC GTESTMAIN_STATIC pthread dl)