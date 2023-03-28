#pragma once

#include <iostream>
// Include these utility headers so we can access utility function from Eigen.
#include "storm/utility/constants.h"

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-pragmas"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#if __GNUC__ > 8
#pragma GCC diagnostic ignored "-Wdeprecated-copy"  // Ignore warnings present for GCC 9 and later. The warnings should be fixed for Eigen versions after 3.3.7
#endif
#endif

// Finally include the parts of Eigen we need.
// Make sure to include our patched version of Eigen (and not a pre-installed one e.g. located at /usr/include)
#include <resources/3rdparty/StormEigen/Eigen/Dense>
#include <resources/3rdparty/StormEigen/Eigen/Sparse>
#include <resources/3rdparty/StormEigen/unsupported/Eigen/IterativeSolvers>

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
