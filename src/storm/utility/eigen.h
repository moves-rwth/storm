#pragma once

// Include this utility header so we can access utility function from Eigen.
#include "storm/utility/constants.h"

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-pragmas"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-attributes"
#pragma GCC diagnostic ignored "-Wmisleading-indentation"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

// Finally include the parts of Eigen we need.
#include <StormEigen/Dense>
#include <StormEigen/Sparse>
#include <unsupported/StormEigen/IterativeSolvers>

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
