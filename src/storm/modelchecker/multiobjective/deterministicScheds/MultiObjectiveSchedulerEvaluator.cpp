#include "storm/modelchecker/multiobjective/deterministicScheds/MultiObjectiveSchedulerEvaluator.h"

namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            template <class ModelType>
            MultiObjectiveSchedulerEvaluator<ModelType>::MultiObjectiveSchedulerEvaluator(preprocessing::SparseMultiObjectivePreprocessorResult<ModelType>& preprocessorResult) {
                // TODO
            }
            
            template <class ModelType>
            void MultiObjectiveSchedulerEvaluator<ModelType>::check(std::vector<uint64_t> const& scheduler) {
                // TODO
            }
 
            template <class ModelType>
            std::vector<typename MultiObjectiveSchedulerEvaluator<ModelType>::ValueType> const& MultiObjectiveSchedulerEvaluator<ModelType>::getResultForObjective(uint64_t objIndex) const {
                return results[objIndex];
            }
            
            template <class ModelType>
            std::vector<ValueType> MultiObjectiveSchedulerEvaluator<ModelType>::getInitialStateResults() const {
                std::vector<ValueType> res;
                for (auto objResult : results) {
                    res.push_back(objResult[initialState]);
                }
                return res;
            }
        }
    }
}