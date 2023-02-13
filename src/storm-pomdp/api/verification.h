#include "storm-pomdp/modelchecker/BeliefExplorationPomdpModelChecker.h"
namespace storm {
namespace pomdp {
namespace api {

    template<typename ValueType>
    typename storm::pomdp::modelchecker::BeliefExplorationPomdpModelChecker<storm::models::sparse::Pomdp<ValueType>>::Result
    underapproximateWithCutoffs(storm::Environment const& env, std::shared_ptr<storm::models::sparse::Pomdp<ValueType>> pomdp,
                              storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task, uint64_t sizeThreshold,  std::vector<std::vector<std::unordered_map<uint64_t,ValueType>>> pomdpStateValues = std::vector<std::vector<std::unordered_map<uint64_t,ValueType>>>()){
        storm::pomdp::modelchecker::BeliefExplorationPomdpModelCheckerOptions<ValueType> options(false,true);
        options.useGridClipping = false;
        options.useStateEliminationCutoff = false;
        options.sizeThresholdInit = sizeThreshold;
        storm::pomdp::modelchecker::BeliefExplorationPomdpModelChecker<storm::models::sparse::Pomdp<ValueType>> modelchecker(pomdp, options);
        return modelchecker.check(task.getFormula(), pomdpStateValues);
    }

    template<typename ValueType>
    typename storm::pomdp::modelchecker::BeliefExplorationPomdpModelChecker<storm::models::sparse::Pomdp<ValueType>>::Result
    underapproximateWithoutHeuristicValues(storm::Environment const& env, std::shared_ptr<storm::models::sparse::Pomdp<ValueType>> pomdp,
                                storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task, uint64_t sizeThreshold,  std::vector<std::vector<std::unordered_map<uint64_t,ValueType>>> pomdpStateValues){
        storm::pomdp::modelchecker::BeliefExplorationPomdpModelCheckerOptions<ValueType> options(false,true);
        options.skipHeuristicSchedulers = true;
        options.useGridClipping = false;
        options.useExplicitCutoff = true;
        options.sizeThresholdInit = sizeThreshold;
        storm::pomdp::modelchecker::BeliefExplorationPomdpModelChecker<storm::models::sparse::Pomdp<ValueType>> modelchecker(pomdp, options);
        return modelchecker.check(task.getFormula(), pomdpStateValues);
    }

    template<typename ValueType>
    storm::pomdp::modelchecker::BeliefExplorationPomdpModelChecker<storm::models::sparse::Pomdp<ValueType>>
    createInteractiveUnfoldingModelChecker(storm::Environment const& env, std::shared_ptr<storm::models::sparse::Pomdp<ValueType>> pomdp, bool useClipping){
        storm::pomdp::modelchecker::BeliefExplorationPomdpModelCheckerOptions<ValueType> options(false,true);
        options.skipHeuristicSchedulers = false;
        options.useGridClipping = useClipping;
        options.useExplicitCutoff = true;
        options.sizeThresholdInit = storm::utility::infinity<ValueType>();
        options.interactiveUnfolding = true;
        options.refine = false;
        options.gapThresholdInit = 0;
        options.cutZeroGap = false;
        storm::pomdp::modelchecker::BeliefExplorationPomdpModelChecker<storm::models::sparse::Pomdp<ValueType>> modelchecker(pomdp, options);
        return modelchecker;
    }

    template<typename ValueType>
    void startInteractiveExploration(storm::pomdp::modelchecker::BeliefExplorationPomdpModelChecker<storm::models::sparse::Pomdp<ValueType>> & modelchecker, storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task, std::vector<std::vector<std::unordered_map<uint64_t,ValueType>>> pomdpStateValues){
         modelchecker.check(task.getFormula(), pomdpStateValues);
    }

    template<typename ValueType>
    std::shared_ptr<storm::models::sparse::Model<ValueType>> extractSchedulerAsMarkovChain(typename storm::pomdp::modelchecker::BeliefExplorationPomdpModelChecker<storm::models::sparse::Pomdp<ValueType>>::Result modelcheckingResult){
        return modelcheckingResult.schedulerAsMarkovChain;
    }

    template<typename ValueType>
    storm::storage::Scheduler<ValueType> getCutoffScheduler(typename storm::pomdp::modelchecker::BeliefExplorationPomdpModelChecker<storm::models::sparse::Pomdp<ValueType>>::Result modelcheckingResult, uint64_t schedId){
        return modelcheckingResult.cutoffSchedulers.at(schedId);
    }

    template<typename ValueType>
    uint64_t getNumberOfPreprocessingSchedulers(typename storm::pomdp::modelchecker::BeliefExplorationPomdpModelChecker<storm::models::sparse::Pomdp<ValueType>>::Result modelcheckingResult){
        return modelcheckingResult.cutoffSchedulers.size();
    }
}
}
}