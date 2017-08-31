#pragma once

#include <memory>

#include "storm/modelchecker/results/CheckResult.h"
#include "storm/modelchecker/multiobjective/SparseMultiObjectivePreprocessorResult.h"
#include "storm/storage/expressions/ExpressionManager.h"

namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            /*
             * This class represents a multi-objective  query for the constraint based approach (using SMT or LP solvers).
             */
            template <class SparseModelType>
            class SparseCbQuery {
            public:
                
                
                virtual ~SparseCbQuery() = default;
                
                /*
                 * Invokes the computation and retrieves the result
                 */
                virtual std::unique_ptr<CheckResult> check() = 0;
                
            protected:
                
                SparseCbQuery(SparseMultiObjectivePreprocessorResult<SparseModelType>& preprocessorResult);
                
                SparseModelType const& originalModel;
                storm::logic::MultiObjectiveFormula const& originalFormula;
                
                SparseModelType preprocessedModel;
                std::vector<Objective<typename SparseModelType::ValueType>> objectives;
                
                std::shared_ptr<storm::expressions::ExpressionManager> expressionManager;
                
                storm::storage::BitVector possibleBottomStates;
                
            };
            
        }
    }
}
