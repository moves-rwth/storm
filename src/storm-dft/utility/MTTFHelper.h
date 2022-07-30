#pragma once

#include "storm-dft/storage/DFT.h"

namespace storm::dft {
namespace utility {

/**
 * Tries to numerically approximate the mttf of the given dft
 * by integrating 1 - cdf(dft) with Simpson's rule
 */
double MTTFHelperProceeding(std::shared_ptr<storm::dft::storage::DFT<double>> const dft, double const stepsize = 1e-10, double const precision = 1e-12);

/**
 * Tries to numerically approximate the mttf of the given dft
 * by integrating 1 - cdf(dft) by changing the variable
 * such that the interval is (0,1) instead of (0,oo)
 */
double MTTFHelperVariableChange(std::shared_ptr<storm::dft::storage::DFT<double>> const dft, double const stepsize = 1e-6);

}  // namespace utility
}  // namespace storm::dft
