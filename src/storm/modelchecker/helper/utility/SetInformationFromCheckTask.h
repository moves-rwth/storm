#pragma once

#include "storm/modelchecker/CheckTask.h"

namespace storm {
namespace modelchecker {
namespace helper {

/*!
 * Forwards relevant information stored in the given CheckTask to the given helper
 */
template<typename HelperType, typename FormulaType, typename ModelType>
void setInformationFromCheckTaskNondeterministic(HelperType& helper,
                                                 storm::modelchecker::CheckTask<FormulaType, typename ModelType::ValueType> const& checkTask,
                                                 ModelType const& model) {
    // Relevancy of initial states.
    if (checkTask.isOnlyInitialStatesRelevantSet()) {
        helper.setRelevantStates(model.getInitialStates());
    }
    // Value threshold to which the result will be compared
    if (checkTask.isBoundSet()) {
        helper.setValueThreshold(checkTask.getBoundComparisonType(), checkTask.getBoundThreshold());
    }
    // Optimization direction
    if (checkTask.isOptimizationDirectionSet()) {
        helper.setOptimizationDirection(checkTask.getOptimizationDirection());
    }
    // Scheduler Production
    helper.setProduceScheduler(checkTask.isProduceSchedulersSet());

    // Qualitative flag
    helper.setQualitative(checkTask.isQualitativeSet());
}

/*!
 * Forwards relevant information stored in the given CheckTask to the given helper
 */
template<typename HelperType, typename FormulaType, typename ModelType>
void setInformationFromCheckTaskDeterministic(HelperType& helper, storm::modelchecker::CheckTask<FormulaType, typename ModelType::ValueType> const& checkTask,
                                              ModelType const& model) {
    // Relevancy of initial states.
    if (checkTask.isOnlyInitialStatesRelevantSet()) {
        helper.setRelevantStates(model.getInitialStates());
    }
    // Value threshold to which the result will be compared
    if (checkTask.isBoundSet()) {
        helper.setValueThreshold(checkTask.getBoundComparisonType(), checkTask.getBoundThreshold());
    }

    // Qualitative flag
    helper.setQualitative(checkTask.isQualitativeSet());
}
}  // namespace helper
}  // namespace modelchecker
}  // namespace storm