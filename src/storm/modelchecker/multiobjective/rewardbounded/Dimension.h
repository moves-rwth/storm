#pragma once

#include <boost/optional.hpp>

#include "storm/storage/BitVector.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/modelchecker/multiobjective/Objective.h"
#include "storm/modelchecker/multiobjective/rewardbounded/EpochManager.h"
#include "storm/modelchecker/multiobjective/rewardbounded/ProductModel.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/utility/vector.h"
#include "storm/storage/memorystructure/MemoryStructure.h"
#include "storm/utility/Stopwatch.h"

namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            template<typename ValueType>
            struct Dimension {
                std::shared_ptr<storm::logic::Formula const> formula;
                uint64_t objectiveIndex;
                boost::optional<std::string> memoryLabel;
                bool isUpperBounded;
                ValueType scalingFactor;
                boost::optional<uint64_t> maxValue;
            };
        }
    }
}