#
# FindCusp
#
# This module finds the CUSP header files and extracts their version.  It
# sets the following variables.
#
# CUSP_INCLUDE_DIR -  Include directory for cusp header files.  (All header
#                       files will actually be in the cusp subdirectory.)
# CUSP_VERSION -      Version of cusp in the form "major.minor.patch".
#
# CUSP_FOUND - Indicates whether Cusp has been found
#

find_path(CUSP_INCLUDE_DIR
	HINTS
		/usr/include/cusp
		/usr/local/include
		/usr/local/cusp/include
		${CUSP_INCLUDE_DIRS}
		${CUSP_HINT}
	NAMES cusp/version.h
	DOC "Cusp headers"
)
if(CUSP_INCLUDE_DIR)
	list(REMOVE_DUPLICATES CUSP_INCLUDE_DIR)
		
	# Find cusp version
	file(STRINGS ${CUSP_INCLUDE_DIR}/cusp/version.h
		version
		REGEX "#define CUSP_VERSION[ \t]+([0-9x]+)"
	)
	string(REGEX REPLACE
		"#define CUSP_VERSION[ \t]+"
		""
		version
		"${version}"
	)

	#define CUSP_MAJOR_VERSION     (CUSP_VERSION / 100000)
	#define CUSP_MINOR_VERSION     (CUSP_VERSION / 100 % 1000)
	#define CUSP_SUBMINOR_VERSION  (CUSP_VERSION % 100)

	math(EXPR CUSP_MAJOR_VERSION "${version} / 100000")
	math(EXPR CUSP_MINOR_VERSION "${version} / 100 % 1000")
	math(EXPR CUSP_PATCH_VERSION "${version} % 100")

	set(CUSP_VERSION "${CUSP_MAJOR_VERSION}.${CUSP_MINOR_VERSION}.${CUSP_PATCH_VERSION}")

	# Check for required components
	include(FindPackageHandleStandardArgs)
	find_package_handle_standard_args(Cusp REQUIRED_VARS CUSP_INCLUDE_DIR VERSION_VAR CUSP_VERSION)

	set(CUSP_INCLUDE_DIRS ${CUSP_INCLUDE_DIR})
	mark_as_advanced(CUSP_INCLUDE_DIR)
	
endif(CUSP_INCLUDE_DIR)