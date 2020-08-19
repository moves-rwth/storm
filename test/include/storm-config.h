/*
 * StoRM - Build-in Options
 *
 * This file is parsed by CMake during makefile generation
 * It contains information such as the base path to the test/example data
 */

#ifndef STORM_GENERATED_STORMCONFIG_H_
#define STORM_GENERATED_STORMCONFIG_H_

// The directory of the sources from which Storm was built.
#define STORM_SOURCE_DIR "/Users/jipspel/Documents/Tools/storm"

// The directory of the test resources used in the tests (model files, ...).
#define STORM_TEST_RESOURCES_DIR "/Users/jipspel/Documents/Tools/storm/resources/examples/testfiles"

// The directory in which Storm was built.
#define STORM_BUILD_DIR "/Users/jipspel/Documents/Tools/storm/test"

// Boost include directory used during compilation.
#define STORM_BOOST_INCLUDE_DIR "/usr/local/include"

// Carl include directory used during compilation.
#define STORM_CARL_INCLUDE_DIR "/Users/jipspel/Documents/Tools/storm/debug-build/resources/3rdparty/carl_download/source_dir/src"

// Whether Gurobi is available and to be used (define/undef)
/* #undef STORM_HAVE_GUROBI */

// Whether CUDA is available (define/undef)
#undef STORM_HAVE_CUDA

// Whether GLPK is available and to be used (define/undef)
#define STORM_HAVE_GLPK

// Whether CudaForStorm is available and to be used (define/undef)
#undef STORM_HAVE_CUDAFORSTORM

// Whether Z3 is available and to be used (define/undef)
#define STORM_HAVE_Z3

// Whether the optimization feature of Z3 is available and to be used (define/undef)
#define STORM_HAVE_Z3_OPTIMIZE

// Version of Z3 used by Storm.
#define STORM_Z3_VERSION_MAJOR 4
#define STORM_Z3_VERSION_MINOR 8
#define STORM_Z3_VERSION_PATCH 8
#define STORM_Z3_VERSION 4.8.8
#define STORM_Z3_API_USES_STANDARD_INTEGERS

// Whether MathSAT is available and to be used (define/undef)
/* #undef STORM_HAVE_MSAT */

// Whether benchmarks from QVBS can be used as input
/* #undef STORM_HAVE_QVBS */

// The root directory of QVBS
/* #undef STORM_QVBS_ROOT */

// Whether Intel Threading Building Blocks are available and to be used (define/undef)
/* #undef STORM_HAVE_INTELTBB */

// Whether support for parametric systems should be enabled
/* #undef PARAMETRIC_SYSTEMS */

// Whether CLN is available and to be used (define/undef)
#define STORM_HAVE_CLN

// Include directory for CLN headers
#define CLN_INCLUDE_DIR "/usr/local/include"

// Whether GMP is available  (it is always available nowadays)
#define STORM_HAVE_GMP

// Include directory for GMP headers
#define GMP_INCLUDE_DIR "/usr/local/include"
#define GMPXX_INCLUDE_DIR "/usr/local/include"

// Whether carl is available and to be used.
#define STORM_HAVE_CARL

/* #undef STORM_USE_CLN_EA */

#define STORM_USE_CLN_RF

#define STORM_HAVE_XERCES

// Whether smtrat is available and to be used.
/* #undef STORM_HAVE_SMTRAT */

// Whether HyPro is available and to be used.
/* #undef STORM_HAVE_HYPRO */

/* #undef STORM_LOGGING_FRAMEWORK */

#define STORM_LOG_DISABLE_DEBUG

#endif // STORM_GENERATED_STORMCONFIG_H_
