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
                    std::shared_ptr<storm::logic::Formula const> formula;
                    uint64_t objectiveIndex;
                    boost::optional<std::string> memoryLabel;
                    bool isUpperBounded;
                    ValueType scalingFactor;
                    storm::storage::BitVector dependentDimensions;
                    boost::optional<uint64_t> maxValue;
                    boost::optional<storm::solver::OptimizationDirection> optimizationDirection;
                };
            }
        }
    }
}