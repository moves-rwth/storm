#include "storm/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"

#include <memory>
#include <vector>

#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/exceptions/InvalidPropertyException.h"
#include "storm/logic/FragmentSpecification.h"
#include "storm/modelchecker/csl/helper/SparseCtmcCslHelper.h"
#include "storm/modelchecker/helper/finitehorizon/SparseDeterministicStepBoundedHorizonHelper.h"
#include "storm/modelchecker/helper/indefinitehorizon/visitingtimes/SparseDeterministicVisitingTimesHelper.h"
#include "storm/modelchecker/helper/infinitehorizon/SparseDeterministicInfiniteHorizonHelper.h"
#include "storm/modelchecker/helper/ltl/SparseLTLHelper.h"
#include "storm/modelchecker/helper/utility/SetInformationFromCheckTask.h"
#include "storm/modelchecker/prctl/helper/SparseDtmcPrctlHelper.h"
#include "storm/modelchecker/prctl/helper/rewardbounded/QuantileHelper.h"
#include "storm/modelchecker/results/ExplicitParetoCurveCheckResult.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/solver/SolveGoal.h"
#include "storm/utility/FilteredRewardModel.h"
#include "storm/utility/macros.h"

namespace storm {
namespace modelchecker {
template<typename SparseDtmcModelType>
SparseDtmcPrctlModelChecker<SparseDtmcModelType>::SparseDtmcPrctlModelChecker(SparseDtmcModelType const& model)
    : SparsePropositionalModelChecker<SparseDtmcModelType>(model) {
    // Intentionally left empty.
}

template<typename SparseDtmcModelType>
bool SparseDtmcPrctlModelChecker<SparseDtmcModelType>::canHandleStatic(CheckTask<storm::logic::Formula, ValueType> const& checkTask,
                                                                       bool* requiresSingleInitialState) {
    storm::logic::Formula const& formula = checkTask.getFormula();
    if (formula.isInFragment(storm::logic::prctlstar()
                                 .setLongRunAverageRewardFormulasAllowed(true)
                                 .setLongRunAverageProbabilitiesAllowed(true)
                                 .setConditionalProbabilityFormulasAllowed(true)
                                 .setConditionalRewardFormulasAllowed(true)
                                 .setTotalRewardFormulasAllowed(true)
                                 .setOnlyEventuallyFormuluasInConditionalFormulasAllowed(true)
                                 .setRewardBoundedUntilFormulasAllowed(true)
                                 .setRewardBoundedCumulativeRewardFormulasAllowed(true)
                                 .setMultiDimensionalBoundedUntilFormulasAllowed(true)
                                 .setMultiDimensionalCumulativeRewardFormulasAllowed(true)
                                 .setTimeOperatorsAllowed(true)
                                 .setReachbilityTimeFormulasAllowed(true)
                                 .setRewardAccumulationAllowed(true)
                                 .setHOAPathFormulasAllowed(true)
                                 .setDiscountedTotalRewardFormulasAllowed(true)
                                 .setDiscountedCumulativeRewardFormulasAllowed(true))) {
        return true;
    } else if (checkTask.isOnlyInitialStatesRelevantSet() && formula.isInFragment(storm::logic::quantiles())) {
        if (requiresSingleInitialState) {
            *requiresSingleInitialState = true;
        }
        return true;
    }
    return false;
}

template<typename SparseDtmcModelType>
bool SparseDtmcPrctlModelChecker<SparseDtmcModelType>::canHandle(CheckTask<storm::logic::Formula, ValueType> const& checkTask) const {
    bool requiresSingleInitialState = false;
    if (canHandleStatic(checkTask, &requiresSingleInitialState)) {
        return !requiresSingleInitialState || this->getModel().getInitialStates().getNumberOfSetBits() == 1;
    } else {
        return false;
    }
}

template<typename SparseDtmcModelType>
std::unique_ptr<CheckResult> SparseDtmcPrctlModelChecker<SparseDtmcModelType>::computeBoundedUntilProbabilities(
    Environment const& env, CheckTask<storm::logic::BoundedUntilFormula, ValueType> const& checkTask) {
    storm::logic::BoundedUntilFormula const& pathFormula = checkTask.getFormula();
    if (pathFormula.isMultiDimensional() || pathFormula.getTimeBoundReference().isRewardBound()) {
        STORM_LOG_THROW(checkTask.isOnlyInitialStatesRelevantSet(), storm::exceptions::InvalidOperationException,
                        "Checking non-trivial bounded until probabilities can only be computed for the initial states of the model.");
        storm::logic::OperatorInformation opInfo;
        if (checkTask.isBoundSet()) {
            opInfo.bound = checkTask.getBound();
        }
        auto formula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(checkTask.getFormula().asSharedPointer(), opInfo);
        auto numericResult = storm::modelchecker::helper::SparseDtmcPrctlHelper<ValueType>::computeRewardBoundedValues(env, this->getModel(), formula);
        return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
    } else {
        STORM_LOG_THROW(pathFormula.hasUpperBound(), storm::exceptions::InvalidPropertyException, "Formula needs to have (a single) upper step bound.");
        STORM_LOG_THROW(pathFormula.hasIntegerLowerBound(), storm::exceptions::InvalidPropertyException, "Formula lower step bound must be discrete/integral.");
        STORM_LOG_THROW(pathFormula.hasIntegerUpperBound(), storm::exceptions::InvalidPropertyException, "Formula needs to have discrete upper step bound.");
        std::unique_ptr<CheckResult> leftResultPointer = this->check(env, pathFormula.getLeftSubformula());
        std::unique_ptr<CheckResult> rightResultPointer = this->check(env, pathFormula.getRightSubformula());
        ExplicitQualitativeCheckResult<ValueType> const& leftResult = leftResultPointer->template asExplicitQualitativeCheckResult<ValueType>();
        ExplicitQualitativeCheckResult<ValueType> const& rightResult = rightResultPointer->template asExplicitQualitativeCheckResult<ValueType>();
        storm::modelchecker::helper::SparseDeterministicStepBoundedHorizonHelper<ValueType> helper;
        std::vector<ValueType> numericResult =
            helper.compute(env, storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(),
                           this->getModel().getBackwardTransitions(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(),
                           pathFormula.getNonStrictLowerBound<uint64_t>(), pathFormula.getNonStrictUpperBound<uint64_t>(), checkTask.getHint());
        std::unique_ptr<CheckResult> result = std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
        return result;
    }
}

template<typename SparseDtmcModelType>
std::unique_ptr<CheckResult> SparseDtmcPrctlModelChecker<SparseDtmcModelType>::computeNextProbabilities(
    Environment const& env, CheckTask<storm::logic::NextFormula, ValueType> const& checkTask) {
    storm::logic::NextFormula const& pathFormula = checkTask.getFormula();
    std::unique_ptr<CheckResult> subResultPointer = this->check(env, pathFormula.getSubformula());
    ExplicitQualitativeCheckResult<ValueType> const& subResult = subResultPointer->template asExplicitQualitativeCheckResult<ValueType>();
    std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseDtmcPrctlHelper<ValueType>::computeNextProbabilities(
        env, this->getModel().getTransitionMatrix(), subResult.getTruthValuesVector());
    return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
}

template<typename SparseDtmcModelType>
std::unique_ptr<CheckResult> SparseDtmcPrctlModelChecker<SparseDtmcModelType>::computeUntilProbabilities(
    Environment const& env, CheckTask<storm::logic::UntilFormula, ValueType> const& checkTask) {
    storm::logic::UntilFormula const& pathFormula = checkTask.getFormula();
    std::unique_ptr<CheckResult> leftResultPointer = this->check(env, pathFormula.getLeftSubformula());
    std::unique_ptr<CheckResult> rightResultPointer = this->check(env, pathFormula.getRightSubformula());
    ExplicitQualitativeCheckResult<ValueType> const& leftResult = leftResultPointer->template asExplicitQualitativeCheckResult<ValueType>();
    ExplicitQualitativeCheckResult<ValueType> const& rightResult = rightResultPointer->template asExplicitQualitativeCheckResult<ValueType>();
    std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseDtmcPrctlHelper<ValueType>::computeUntilProbabilities(
        env, storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(),
        this->getModel().getBackwardTransitions(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), checkTask.isQualitativeSet(),
        checkTask.getHint());
    return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
}

template<typename SparseDtmcModelType>
std::unique_ptr<CheckResult> SparseDtmcPrctlModelChecker<SparseDtmcModelType>::computeGloballyProbabilities(
    Environment const& env, CheckTask<storm::logic::GloballyFormula, ValueType> const& checkTask) {
    storm::logic::GloballyFormula const& pathFormula = checkTask.getFormula();
    std::unique_ptr<CheckResult> subResultPointer = this->check(env, pathFormula.getSubformula());
    ExplicitQualitativeCheckResult<ValueType> const& subResult = subResultPointer->template asExplicitQualitativeCheckResult<ValueType>();
    std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseDtmcPrctlHelper<ValueType>::computeGloballyProbabilities(
        env, storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(),
        this->getModel().getBackwardTransitions(), subResult.getTruthValuesVector(), checkTask.isQualitativeSet());
    return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
}

template<typename SparseDtmcModelType>
std::unique_ptr<CheckResult> SparseDtmcPrctlModelChecker<SparseDtmcModelType>::computeHOAPathProbabilities(
    Environment const& env, CheckTask<storm::logic::HOAPathFormula, ValueType> const& checkTask) {
    storm::logic::HOAPathFormula const& pathFormula = checkTask.getFormula();

    storm::modelchecker::helper::SparseLTLHelper<ValueType, false> helper(this->getModel().getTransitionMatrix());
    storm::modelchecker::helper::setInformationFromCheckTaskDeterministic(helper, checkTask, this->getModel());

    auto formulaChecker = [&](storm::logic::Formula const& formula) {
        return this->check(env, formula)->template asExplicitQualitativeCheckResult<ValueType>().getTruthValuesVector();
    };
    auto apSets = helper.computeApSets(pathFormula.getAPMapping(), formulaChecker);
    std::vector<ValueType> numericResult = helper.computeDAProductProbabilities(env, *pathFormula.readAutomaton(), apSets);

    return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
}

template<typename SparseDtmcModelType>
std::unique_ptr<CheckResult> SparseDtmcPrctlModelChecker<SparseDtmcModelType>::computeLTLProbabilities(
    Environment const& env, CheckTask<storm::logic::PathFormula, ValueType> const& checkTask) {
    storm::logic::PathFormula const& pathFormula = checkTask.getFormula();

    storm::modelchecker::helper::SparseLTLHelper<ValueType, false> helper(this->getModel().getTransitionMatrix());
    storm::modelchecker::helper::setInformationFromCheckTaskDeterministic(helper, checkTask, this->getModel());

    auto formulaChecker = [&](storm::logic::Formula const& formula) {
        return this->check(env, formula)->template asExplicitQualitativeCheckResult<ValueType>().getTruthValuesVector();
    };
    std::vector<ValueType> numericResult = helper.computeLTLProbabilities(env, pathFormula, formulaChecker);

    return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
}

template<typename SparseDtmcModelType>
std::unique_ptr<CheckResult> SparseDtmcPrctlModelChecker<SparseDtmcModelType>::computeCumulativeRewards(
    Environment const& env, CheckTask<storm::logic::CumulativeRewardFormula, ValueType> const& checkTask) {
    storm::logic::CumulativeRewardFormula const& rewardPathFormula = checkTask.getFormula();
    if (rewardPathFormula.isMultiDimensional() || rewardPathFormula.getTimeBoundReference().isRewardBound()) {
        STORM_LOG_THROW(checkTask.isOnlyInitialStatesRelevantSet(), storm::exceptions::InvalidOperationException,
                        "Checking non-trivial bounded until probabilities can only be computed for the initial states of the model.");
        STORM_LOG_THROW(!checkTask.getFormula().hasRewardAccumulation(), storm::exceptions::InvalidOperationException,
                        "Checking reward bounded cumulative reward formulas is not supported if reward accumulations are given.");
        storm::logic::OperatorInformation opInfo;
        if (checkTask.isBoundSet()) {
            opInfo.bound = checkTask.getBound();
        }
        auto formula = std::make_shared<storm::logic::RewardOperatorFormula>(checkTask.getFormula().asSharedPointer(), checkTask.getRewardModel(), opInfo);
        auto numericResult = storm::modelchecker::helper::SparseDtmcPrctlHelper<ValueType>::computeRewardBoundedValues(env, this->getModel(), formula);
        return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
    } else {
        STORM_LOG_THROW(rewardPathFormula.hasIntegerBound(), storm::exceptions::InvalidPropertyException, "Formula needs to have a discrete time bound.");
        auto rewardModel = storm::utility::createFilteredRewardModel(this->getModel(), checkTask);
        std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseDtmcPrctlHelper<ValueType>::computeCumulativeRewards(
            env, storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(), rewardModel.get(),
            rewardPathFormula.getNonStrictBound<uint64_t>());
        return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
    }
}

template<>
std::unique_ptr<CheckResult> SparseDtmcPrctlModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>>::computeDiscountedCumulativeRewards(
    Environment const& env, CheckTask<storm::logic::DiscountedCumulativeRewardFormula, storm::RationalFunction> const& checkTask) {
    STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Discounted properties are not implemented for parametric models.");
}

template<typename SparseDtmcModelType>
std::unique_ptr<CheckResult> SparseDtmcPrctlModelChecker<SparseDtmcModelType>::computeDiscountedCumulativeRewards(
    Environment const& env, CheckTask<storm::logic::DiscountedCumulativeRewardFormula, ValueType> const& checkTask) {
    storm::logic::DiscountedCumulativeRewardFormula const& rewardPathFormula = checkTask.getFormula();
    STORM_LOG_THROW(rewardPathFormula.hasIntegerBound(), storm::exceptions::InvalidPropertyException, "Formula needs to have a discrete time bound.");
    auto rewardModel = storm::utility::createFilteredRewardModel(this->getModel(), checkTask);
    std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseDtmcPrctlHelper<ValueType>::computeDiscountedCumulativeRewards(
        env, storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(), rewardModel.get(),
        rewardPathFormula.getNonStrictBound<uint64_t>(), rewardPathFormula.getDiscountFactor<ValueType>());
    return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
}

template<typename SparseDtmcModelType>
std::unique_ptr<CheckResult> SparseDtmcPrctlModelChecker<SparseDtmcModelType>::computeInstantaneousRewards(
    Environment const& env, CheckTask<storm::logic::InstantaneousRewardFormula, ValueType> const& checkTask) {
    storm::logic::InstantaneousRewardFormula const& rewardPathFormula = checkTask.getFormula();
    STORM_LOG_THROW(rewardPathFormula.hasIntegerBound(), storm::exceptions::InvalidPropertyException, "Formula needs to have a discrete time bound.");
    std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseDtmcPrctlHelper<ValueType>::computeInstantaneousRewards(
        env, storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(),
        checkTask.isRewardModelSet() ? this->getModel().getRewardModel(checkTask.getRewardModel()) : this->getModel().getRewardModel(""),
        rewardPathFormula.getBound<uint64_t>());
    return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
}

template<typename SparseDtmcModelType>
std::unique_ptr<CheckResult> SparseDtmcPrctlModelChecker<SparseDtmcModelType>::computeReachabilityRewards(
    Environment const& env, CheckTask<storm::logic::EventuallyFormula, ValueType> const& checkTask) {
    storm::logic::EventuallyFormula const& eventuallyFormula = checkTask.getFormula();
    std::unique_ptr<CheckResult> subResultPointer = this->check(env, eventuallyFormula.getSubformula());
    ExplicitQualitativeCheckResult<ValueType> const& subResult = subResultPointer->template asExplicitQualitativeCheckResult<ValueType>();
    auto rewardModel = storm::utility::createFilteredRewardModel(this->getModel(), checkTask);
    std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseDtmcPrctlHelper<ValueType>::computeReachabilityRewards(
        env, storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(),
        this->getModel().getBackwardTransitions(), rewardModel.get(), subResult.getTruthValuesVector(), checkTask.isQualitativeSet(), checkTask.getHint());
    return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
}

template<typename SparseDtmcModelType>
std::unique_ptr<CheckResult> SparseDtmcPrctlModelChecker<SparseDtmcModelType>::computeReachabilityTimes(
    Environment const& env, CheckTask<storm::logic::EventuallyFormula, ValueType> const& checkTask) {
    storm::logic::EventuallyFormula const& eventuallyFormula = checkTask.getFormula();
    std::unique_ptr<CheckResult> subResultPointer = this->check(env, eventuallyFormula.getSubformula());
    ExplicitQualitativeCheckResult<ValueType> const& subResult = subResultPointer->template asExplicitQualitativeCheckResult<ValueType>();
    std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseDtmcPrctlHelper<ValueType>::computeReachabilityTimes(
        env, storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(),
        this->getModel().getBackwardTransitions(), subResult.getTruthValuesVector(), checkTask.isQualitativeSet(), checkTask.getHint());
    return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
}

template<typename SparseDtmcModelType>
std::unique_ptr<CheckResult> SparseDtmcPrctlModelChecker<SparseDtmcModelType>::computeTotalRewards(
    Environment const& env, CheckTask<storm::logic::TotalRewardFormula, ValueType> const& checkTask) {
    auto rewardModel = storm::utility::createFilteredRewardModel(this->getModel(), checkTask);
    std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseDtmcPrctlHelper<ValueType>::computeTotalRewards(
        env, storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(),
        this->getModel().getBackwardTransitions(), rewardModel.get(), checkTask.isQualitativeSet(), checkTask.getHint());
    return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
}

template<>
std::unique_ptr<CheckResult> SparseDtmcPrctlModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>>::computeDiscountedTotalRewards(
    Environment const& env, CheckTask<storm::logic::DiscountedTotalRewardFormula, ValueType> const& checkTask) {
    STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Discounted properties are not implemented for parametric models.");
}

template<typename SparseDtmcModelType>
std::unique_ptr<CheckResult> SparseDtmcPrctlModelChecker<SparseDtmcModelType>::computeDiscountedTotalRewards(
    Environment const& env, CheckTask<storm::logic::DiscountedTotalRewardFormula, ValueType> const& checkTask) {
    auto rewardModel = storm::utility::createFilteredRewardModel(this->getModel(), checkTask);
    storm::logic::DiscountedTotalRewardFormula const& rewardPathFormula = checkTask.getFormula();
    auto discountFactor = rewardPathFormula.getDiscountFactor<ValueType>();
    auto ret = storm::modelchecker::helper::SparseDtmcPrctlHelper<ValueType>::computeDiscountedTotalRewards(
        env, storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(),
        this->getModel().getBackwardTransitions(), rewardModel.get(), checkTask.isQualitativeSet(), discountFactor, checkTask.getHint());
    std::unique_ptr<CheckResult> result(new ExplicitQuantitativeCheckResult<ValueType>(std::move(ret)));
    return result;
}

template<typename SparseDtmcModelType>
std::unique_ptr<CheckResult> SparseDtmcPrctlModelChecker<SparseDtmcModelType>::computeLongRunAverageProbabilities(
    Environment const& env, CheckTask<storm::logic::StateFormula, ValueType> const& checkTask) {
    storm::logic::StateFormula const& stateFormula = checkTask.getFormula();
    std::unique_ptr<CheckResult> subResultPointer = this->check(env, stateFormula);
    ExplicitQualitativeCheckResult<ValueType> const& subResult = subResultPointer->template asExplicitQualitativeCheckResult<ValueType>();

    storm::modelchecker::helper::SparseDeterministicInfiniteHorizonHelper<ValueType> helper(this->getModel().getTransitionMatrix());
    storm::modelchecker::helper::setInformationFromCheckTaskDeterministic(helper, checkTask, this->getModel());
    auto values = helper.computeLongRunAverageProbabilities(env, subResult.getTruthValuesVector());

    return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(values)));
}

template<typename SparseDtmcModelType>
std::unique_ptr<CheckResult> SparseDtmcPrctlModelChecker<SparseDtmcModelType>::computeLongRunAverageRewards(
    Environment const& env, CheckTask<storm::logic::LongRunAverageRewardFormula, ValueType> const& checkTask) {
    auto rewardModel = storm::utility::createFilteredRewardModel(this->getModel(), checkTask);
    storm::modelchecker::helper::SparseDeterministicInfiniteHorizonHelper<ValueType> helper(this->getModel().getTransitionMatrix());
    storm::modelchecker::helper::setInformationFromCheckTaskDeterministic(helper, checkTask, this->getModel());
    auto values = helper.computeLongRunAverageRewards(env, rewardModel.get());
    return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(values)));
}

template<typename SparseDtmcModelType>
std::unique_ptr<CheckResult> SparseDtmcPrctlModelChecker<SparseDtmcModelType>::computeConditionalProbabilities(
    Environment const& env, CheckTask<storm::logic::ConditionalFormula, ValueType> const& checkTask) {
    storm::logic::ConditionalFormula const& conditionalFormula = checkTask.getFormula();
    STORM_LOG_THROW(conditionalFormula.getSubformula().isEventuallyFormula(), storm::exceptions::InvalidPropertyException,
                    "Illegal conditional probability formula.");
    STORM_LOG_THROW(conditionalFormula.getConditionFormula().isEventuallyFormula(), storm::exceptions::InvalidPropertyException,
                    "Illegal conditional probability formula.");

    std::unique_ptr<CheckResult> leftResultPointer = this->check(env, conditionalFormula.getSubformula().asEventuallyFormula().getSubformula());
    std::unique_ptr<CheckResult> rightResultPointer = this->check(env, conditionalFormula.getConditionFormula().asEventuallyFormula().getSubformula());
    ExplicitQualitativeCheckResult<ValueType> const& leftResult = leftResultPointer->template asExplicitQualitativeCheckResult<ValueType>();
    ExplicitQualitativeCheckResult<ValueType> const& rightResult = rightResultPointer->template asExplicitQualitativeCheckResult<ValueType>();

    std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseDtmcPrctlHelper<ValueType>::computeConditionalProbabilities(
        env, storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(),
        this->getModel().getBackwardTransitions(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), checkTask.isQualitativeSet());
    return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
}

template<typename SparseDtmcModelType>
std::unique_ptr<CheckResult> SparseDtmcPrctlModelChecker<SparseDtmcModelType>::computeConditionalRewards(
    Environment const& env, CheckTask<storm::logic::ConditionalFormula, ValueType> const& checkTask) {
    storm::logic::ConditionalFormula const& conditionalFormula = checkTask.getFormula();
    STORM_LOG_THROW(conditionalFormula.getSubformula().isReachabilityRewardFormula(), storm::exceptions::InvalidPropertyException,
                    "Illegal conditional probability formula.");
    STORM_LOG_THROW(conditionalFormula.getConditionFormula().isEventuallyFormula(), storm::exceptions::InvalidPropertyException,
                    "Illegal conditional probability formula.");

    std::unique_ptr<CheckResult> leftResultPointer = this->check(env, conditionalFormula.getSubformula().asReachabilityRewardFormula().getSubformula());
    std::unique_ptr<CheckResult> rightResultPointer = this->check(env, conditionalFormula.getConditionFormula().asEventuallyFormula().getSubformula());
    ExplicitQualitativeCheckResult<ValueType> const& leftResult = leftResultPointer->template asExplicitQualitativeCheckResult<ValueType>();
    ExplicitQualitativeCheckResult<ValueType> const& rightResult = rightResultPointer->template asExplicitQualitativeCheckResult<ValueType>();

    std::vector<ValueType> numericResult = storm::modelchecker::helper::SparseDtmcPrctlHelper<ValueType>::computeConditionalRewards(
        env, storm::solver::SolveGoal<ValueType>(this->getModel(), checkTask), this->getModel().getTransitionMatrix(),
        this->getModel().getBackwardTransitions(),
        checkTask.isRewardModelSet() ? this->getModel().getRewardModel(checkTask.getRewardModel()) : this->getModel().getRewardModel(""),
        leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), checkTask.isQualitativeSet());
    return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(numericResult)));
}

template<>
std::unique_ptr<CheckResult> SparseDtmcPrctlModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>>::checkQuantileFormula(
    Environment const& env, CheckTask<storm::logic::QuantileFormula, ValueType> const& checkTask) {
    STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Quantiles for parametric models are not implemented.");
}

template<typename SparseDtmcModelType>
std::unique_ptr<CheckResult> SparseDtmcPrctlModelChecker<SparseDtmcModelType>::checkQuantileFormula(
    Environment const& env, CheckTask<storm::logic::QuantileFormula, ValueType> const& checkTask) {
    STORM_LOG_THROW(checkTask.isOnlyInitialStatesRelevantSet(), storm::exceptions::InvalidOperationException,
                    "Computing quantiles is only supported for the initial states of a model.");
    STORM_LOG_THROW(this->getModel().getInitialStates().getNumberOfSetBits() == 1, storm::exceptions::InvalidOperationException,
                    "Quantiles not supported on models with multiple initial states.");
    uint64_t initialState = *this->getModel().getInitialStates().begin();

    helper::rewardbounded::QuantileHelper<SparseDtmcModelType> qHelper(this->getModel(), checkTask.getFormula());
    auto res = qHelper.computeQuantile(env);

    if (res.size() == 1 && res.front().size() == 1) {
        return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(initialState, std::move(res.front().front())));
    } else {
        return std::unique_ptr<CheckResult>(new ExplicitParetoCurveCheckResult<ValueType>(initialState, std::move(res)));
    }
}

template<typename SparseDtmcModelType>
std::unique_ptr<CheckResult> SparseDtmcPrctlModelChecker<SparseDtmcModelType>::computeSteadyStateDistribution(Environment const& env) {
    // Initialize helper
    storm::modelchecker::helper::SparseDeterministicInfiniteHorizonHelper<ValueType> helper(this->getModel().getTransitionMatrix());

    // Compute result
    std::vector<ValueType> result;
    auto const& initialStates = this->getModel().getInitialStates();
    uint64_t numInitStates = initialStates.getNumberOfSetBits();
    if (numInitStates == 1) {
        result = helper.computeLongRunAverageStateDistribution(env, *initialStates.begin());
    } else {
        STORM_LOG_WARN("Multiple initial states found. A uniform distribution over initial states is assumed.");
        ValueType initProb = storm::utility::one<ValueType>() / storm::utility::convertNumber<ValueType, uint64_t>(numInitStates);
        result = helper.computeLongRunAverageStateDistribution(env, [&initialStates, &initProb](uint64_t const& stateIndex) {
            return initialStates.get(stateIndex) ? initProb : storm::utility::zero<ValueType>();
        });
    }

    // Return CheckResult
    return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(result)));
}

template<typename SparseDtmcModelType>
std::unique_ptr<CheckResult> SparseDtmcPrctlModelChecker<SparseDtmcModelType>::computeExpectedVisitingTimes(Environment const& env) {
    // Initialize helper
    storm::modelchecker::helper::SparseDeterministicVisitingTimesHelper<ValueType> helper(this->getModel().getTransitionMatrix());

    // Compute result
    std::vector<ValueType> result;
    auto const& initialStates = this->getModel().getInitialStates();
    uint64_t numInitStates = initialStates.getNumberOfSetBits();
    STORM_LOG_THROW(numInitStates > 0, storm::exceptions::InvalidOperationException, "No initial states given. Cannot compute expected visiting times.");
    STORM_LOG_WARN_COND(numInitStates == 1, "Multiple initial states found. A uniform distribution over initial states is assumed.");
    result = helper.computeExpectedVisitingTimes(env, initialStates);

    // Return CheckResult
    return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::move(result)));
}

template class SparseDtmcPrctlModelChecker<storm::models::sparse::Dtmc<double>>;

#ifdef STORM_HAVE_CARL
template class SparseDtmcPrctlModelChecker<storm::models::sparse::Dtmc<storm::RationalNumber>>;
template class SparseDtmcPrctlModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>>;
#endif
}  // namespace modelchecker
}  // namespace storm
