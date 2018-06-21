#pragma once

#include <vector>

#include "storm/modelchecker/multiobjective/preprocessing/SparseMultiObjectivePreprocessorResult.h"

namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            template <class ModelType>
            class MultiObjectiveSchedulerEvaluator {
            public:
                
                typedef typename ModelType::ValueType ValueType;
                
                MultiObjectiveSchedulerEvaluator(preprocessing::SparseMultiObjectivePreprocessorResult<ModelType>& preprocessorResult);

                void check(std::vector<uint64_t> const& scheduler);
                
                std::vector<ValueType> const& getResultForObjective(uint64_t objIndex) const;
                std::vector<ValueType> getInitialStateResults() const;
                
            private:
                std::vector<std::vector<ValueType>> results;
                uint64_t initialState;
            };
        }
    }
}