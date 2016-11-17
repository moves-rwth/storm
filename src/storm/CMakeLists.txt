#############################################################
##
##	Source file aggregation and clustering
##
#############################################################
file(GLOB_RECURSE STORM_HEADERS ${PROJECT_SOURCE_DIR}/src/storm/*.h)
file(GLOB_RECURSE STORM_SOURCES_WITHOUT_MAIN ${PROJECT_SOURCE_DIR}/src/storm/*/*.cpp)
file(GLOB_RECURSE STORM_MAIN_FILE ${PROJECT_SOURCE_DIR}/src/storm/storm.cpp)
file(GLOB_RECURSE STORM_DFT_MAIN_FILE ${PROJECT_SOURCE_DIR}/src/storm/storm-dyftee.cpp)
file(GLOB_RECURSE STORM_GSPN_MAIN_FILE ${PROJECT_SOURCE_DIR}/src/storm/storm-gspn.cpp)

# Additional include files like the storm-config.h
file(GLOB_RECURSE STORM_BUILD_HEADERS ${PROJECT_BINARY_DIR}/include/*.h)

set(STORM_LIB_SOURCES ${STORM_SOURCES_WITHOUT_MAIN})
set(STORM_LIB_HEADERS ${STORM_HEADERS})
set(STORM_MAIN_SOURCES  ${STORM_MAIN_FILE})
set(STORM_DFT_MAIN_SOURCES ${STORM_DFT_MAIN_FILE})
set(STORM_GSPN_MAIN_SOURCES ${STORM_GSPN_MAIN_FILE})

# Add custom additional include or link directories
if (ADDITIONAL_INCLUDE_DIRS)
	message(STATUS "StoRM - Using additional include directories ${ADDITIONAL_INCLUDE_DIRS}")
	include_directories(${ADDITIONAL_INCLUDE_DIRS})
endif(ADDITIONAL_INCLUDE_DIRS)
if (ADDITIONAL_LINK_DIRS)
	message(STATUS "StoRM - Using additional link directories ${ADDITIONAL_LINK_DIRS}")
	link_directories(${ADDITIONAL_LINK_DIRS})
endif(ADDITIONAL_LINK_DIRS)

###############################################################################
##                                                                            
##	Binary creation (All link_directories() calls must be made before this point.)
##
###############################################################################

# Create libstorm.
add_library(storm SHARED ${STORM_LIB_SOURCES} ${STORM_LIB_HEADERS})
# Remove define symbol for shared libstorm.
set_target_properties(storm PROPERTIES DEFINE_SYMBOL "")
add_dependencies(storm resources)
target_link_libraries(storm ${STORM_LINK_LIBRARIES})

# Create storm.
add_executable(storm-main ${STORM_MAIN_SOURCES} ${STORM_MAIN_HEADERS})
target_link_libraries(storm-main storm)
set_target_properties(storm-main PROPERTIES OUTPUT_NAME "storm")

# Create storm-dft.
add_executable(storm-dft-main ${STORM_DFT_MAIN_SOURCES} ${STORM_MAIN_HEADERS})
target_link_libraries(storm-dft-main storm) # Adding headers for xcode
set_target_properties(storm-dft-main PROPERTIES OUTPUT_NAME "storm-dft")

add_executable(storm-gspn-main ${STORM_GSPN_MAIN_SOURCES} ${STORM_MAIN_HEADERS})
target_link_libraries(storm-gspn-main storm) # Adding headers for xcode
set_target_properties(storm-gspn-main PROPERTIES OUTPUT_NAME "storm-gspn")

INSTALL(TARGETS storm-main
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)
