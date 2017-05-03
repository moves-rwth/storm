#pragma once

#include "storm/modelchecker/multiobjective/constraintbased/SparseCbQuery.h"

namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            /*
             * This class represents an achievability query for the constraint based approach (using SMT or LP solvers).
             */
            template <class SparseModelType>
            class SparseCbAchievabilityQuery : public SparseCbQuery<SparseModelType> {
            public:
                
                SparseCbAchievabilityQuery(SparseMultiObjectivePreprocessorReturnType<SparseModelType>& preprocessorResult);
                
                virtual ~SparseCbAchievabilityQuery() = default;

                /*
                 * Invokes the computation and retrieves the result
                 */
                virtual std::unique_ptr<CheckResult> check() override;
                
            private:
                
                /*
                 * Returns whether the given thresholds are achievable.
                 */
                bool checkAchievability();
                
            };
            
        }
    }
}
