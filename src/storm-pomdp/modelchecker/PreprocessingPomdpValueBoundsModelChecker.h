#pragma once

#include "solver/SolverSelectionOptions.h"
#include "storm-pomdp/analysis/FormulaInformation.h"
#include "storm-pomdp/storage/BeliefExplorationBounds.h"
#include "storm/api/verification.h"
#include "storm/models/sparse/Pomdp.h"

namespace storm {
class Environment;
namespace modelchecker {
template<typename FormulaType, typename ValueType>
class CheckTask;
class CheckResult;
}  // namespace modelchecker
namespace logic {
class Formula;
}
namespace pomdp {
namespace modelchecker {
template<typename ValueType>
class PreprocessingPomdpValueBoundsModelChecker {
   public:
    typedef pomdp::storage::PreprocessingPomdpValueBounds<ValueType> ValueBounds;
    typedef pomdp::storage::ExtremePOMDPValueBound<ValueType> ExtremeValueBound;

    PreprocessingPomdpValueBoundsModelChecker(storm::models::sparse::Pomdp<ValueType> const& pomdp);

    ValueBounds getValueBounds(storm::logic::Formula const& formula);

    ValueBounds getValueBounds(storm::Environment const& env, storm::logic::Formula const& formula);

    ValueBounds getValueBounds(storm::Environment const& env, storm::logic::Formula const& formula, storm::pomdp::analysis::FormulaInformation const& info);

    ExtremeValueBound getExtremeValueBound(storm::logic::Formula const& formula);

    ExtremeValueBound getExtremeValueBound(storm::Environment const& env, storm::logic::Formula const& formula);

    ExtremeValueBound getExtremeValueBound(storm::Environment const& env, storm::logic::Formula const& formula,
                                           storm::pomdp::analysis::FormulaInformation const& info);

   private:
    storm::models::sparse::Pomdp<ValueType> const& pomdp;

    std::vector<ValueType> getChoiceValues(std::vector<ValueType> const& stateValues, std::vector<ValueType>* actionBasedRewards);

    std::pair<std::vector<ValueType>, storm::storage::Scheduler<ValueType>> computeValuesForGuessedScheduler(
        storm::Environment const& env, std::vector<ValueType> const& stateValues, std::vector<ValueType>* actionBasedRewards,
        storm::logic::Formula const& formula, storm::pomdp::analysis::FormulaInformation const& info,
        std::shared_ptr<storm::models::sparse::Mdp<ValueType>> underlyingMdp, ValueType const& scoreThreshold, bool relativeScore);

    std::pair<std::vector<ValueType>, storm::storage::Scheduler<ValueType>> computeValuesForRandomFMPolicy(
        storm::Environment const& env, storm::logic::Formula const& formula, storm::pomdp::analysis::FormulaInformation const& info, uint64_t memoryBound);

    std::pair<std::vector<ValueType>, storm::storage::Scheduler<ValueType>> computeValuesForRandomMemorylessPolicy(
        storm::Environment const& env, storm::logic::Formula const& formula, storm::pomdp::analysis::FormulaInformation const& info,
        std::shared_ptr<storm::models::sparse::Mdp<ValueType>> underlyingMdp);
};
}  // namespace modelchecker
}  // namespace pomdp
}  // namespace storm