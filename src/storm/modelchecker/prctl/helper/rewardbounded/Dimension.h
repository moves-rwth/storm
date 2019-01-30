#pragma once

#include <boost/optional.hpp>

#include "storm/storage/BitVector.h"
#include "storm/solver/OptimizationDirection.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
            namespace rewardbounded {
                
                template<typename ValueType>
                struct Dimension {
                    /// The formula describing this dimension
                    std::shared_ptr<storm::logic::Formula const> formula;

                    /// The index of the associated objective
                    uint64_t objectiveIndex;

                    /// A label that indicates the states where this dimension is still relevant (i.e., it is yet unknown whether the corresponding objective holds)
                    boost::optional<std::string> memoryLabel;

                    /// True iff the objective is not bounded at all (i.e., it has lower bound >= 0)
                    bool isNotBounded;

                    /// True iff the objective is upper bounded, false if it has a lower bound or no bound at all.
                    bool isUpperBounded;

                    /// Multiplying an epoch value with this factor yields the reward/cost in the original domain.
                    ValueType scalingFactor;

                    /// The dimensions that are not satisfiable whenever the bound of this dimension is violated
                    storm::storage::BitVector dependentDimensions;

                    /// The maximal epoch value that needs to be considered for this dimension
                    boost::optional<uint64_t> maxValue;

                    /// Whether we minimize/maximize the objective for this dimension
                    boost::optional<storm::solver::OptimizationDirection> optimizationDirection;
                };
            }
        }
    }
}