#include "storm-pomdp/modelchecker/BeliefExplorationPomdpModelChecker.h"
namespace storm {
namespace pomdp {
namespace api {

    template<typename ValueType>
    typename storm::pomdp::modelchecker::BeliefExplorationPomdpModelChecker<ValueType>::Result
    underapproximateWithCutoffs(storm::Environment const& env, std::shared_ptr<storm::models::sparse::Pomdp<ValueType>> pomdp,
                              storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task, uint64_t sizeThreshold){
        storm::pomdp::modelchecker::BeliefExplorationPomdpModelCheckerOptions<ValueType> options(false,true);
        options.useExplicitCutoff = true;
        options.sizeThresholdInit = sizeThreshold;
        storm::pomdp::modelchecker::BeliefExplorationPomdpModelChecker<ValueType> modelchecker(pomdp, options);
        return modelchecker.check(task.getFormula());
    }

    template<typename ValueType>
    std::shared_ptr<storm::models::sparse::Model<ValueType>> extractSchedulerAsMarkovChain(typename storm::pomdp::modelchecker::BeliefExplorationPomdpModelChecker<ValueType>::Result modelcheckingResult){
        return modelcheckingResult.schedulerAsMarkovChain;
    }
}
}
}