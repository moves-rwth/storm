#pragma once

// Include this utility header so we can access utility function from Eigen.
#include "storm/utility/constants.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-pragmas"

// Finally include the parts of Eigen we need.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-attributes"
#pragma GCC diagnostic ignored "-Wmisleading-indentation"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <StormEigen/Dense>
#include <StormEigen/Sparse>
#include <unsupported/StormEigen/IterativeSolvers>
#pragma GCC diagnostic pop
#pragma clang diagnostic pop
