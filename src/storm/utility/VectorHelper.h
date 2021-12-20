#pragma once

#include <cstdint>
#include <vector>

#include "storm/solver/OptimizationDirection.h"

namespace storm {
namespace utility {

template<typename ValueType>
class VectorHelper {
   public:
    VectorHelper();

    void reduceVector(storm::solver::OptimizationDirection dir, std::vector<ValueType> const& source, std::vector<ValueType>& target,
                      std::vector<uint_fast64_t> const& rowGrouping, std::vector<uint_fast64_t>* choices = nullptr) const;

    bool parallelize() const;

   private:
    // A flag that stores whether the parallelization to be used.
    bool doParallelize;
};
}  // namespace utility
}  // namespace storm
