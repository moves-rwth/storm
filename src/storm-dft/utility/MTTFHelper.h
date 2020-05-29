#pragma once

#include <cmath>
#include <vector>

#include "storm-dft/modelchecker/dft/DFTModularizer.h"

namespace storm {
namespace dft {
namespace utility {

/**
 * Tries to numerically approximate the mttf of the given dft
 * by integrating 1 - cdf(dft) with Simpson's rule
 */
double MTTFHelper(std::shared_ptr<storm::storage::DFT<double>> const dft,
                  double const stepsize = 0.01);

}  // namespace utility
}  // namespace dft
}  // namespace storm
