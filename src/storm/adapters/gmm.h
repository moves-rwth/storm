#pragma once
#include "storm-config.h"

#ifdef STORM_HAVE_GMM

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wunknown-pragmas"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic ignored "-Wmisleading-indentation"
#endif

#include <gmm/gmm_kernel.h>

#include <gmm/gmm_iter.h>
#include <gmm/gmm_matrix.h>

#include <gmm/gmm_precond_diagonal.h>
#include <gmm/gmm_precond_ilu.h>
#include <gmm/gmm_solver_bicgstab.h>
#include <gmm/gmm_solver_gmres.h>
#include <gmm/gmm_solver_qmr.h>

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#endif
