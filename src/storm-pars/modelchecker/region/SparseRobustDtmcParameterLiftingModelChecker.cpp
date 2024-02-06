#include "storm-pars/modelchecker/region/SparseRobustDtmcParameterLiftingModelChecker.h"

#include "storage/BitVector.h"
#include "storm-pars/transformer/SparseParametricDtmcSimplifier.h"

#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/modelchecker/prctl/helper/BaierUpperRewardBoundsComputer.h"
#include "storm/modelchecker/prctl/helper/DsMpiUpperRewardBoundsComputer.h"
#include "storm/modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/solver/MinMaxLinearEquationSolver.h"
#include "storm/solver/multiplier/Multiplier.h"
#include "storm/utility/NumberTraits.h"
#include "storm/utility/graph.h"
#include "storm/utility/vector.h"

#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/InvalidOperationException.h"
#include "storm/exceptions/InvalidPropertyException.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/UncheckedRequirementException.h"
#include "storm/exceptions/UnexpectedException.h"
#include "utility/constants.h"
#include "utility/logging.h"

namespace storm {
namespace modelchecker {

template<typename SparseModelType, typename ConstantType>
SparseRobustDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::SparseRobustDtmcParameterLiftingModelChecker()
    : SparseRobustDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>(
          std::make_unique<storm::solver::GeneralMinMaxLinearEquationSolverFactory<Interval, double>>()) {
    // Intentionally left empty
}

template<typename SparseModelType, typename ConstantType>
SparseRobustDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::SparseRobustDtmcParameterLiftingModelChecker(
    std::unique_ptr<storm::solver::MinMaxLinearEquationSolverFactory<Interval, double>>&& solverFactory)
    : solverFactory(std::move(solverFactory)), solvingRequiresUpperRewardBounds(false), regionSplitEstimationsEnabled(false) {
    // Intentionally left empty
}

template<typename SparseModelType, typename ConstantType>
bool SparseRobustDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::canHandle(
    std::shared_ptr<storm::models::ModelBase> parametricModel, CheckTask<storm::logic::Formula, ValueType> const& checkTask) const {
    bool result = parametricModel->isOfType(storm::models::ModelType::Dtmc);
    result &= parametricModel->isSparseModel();
    result &= parametricModel->supportsParameters();
    auto dtmc = parametricModel->template as<SparseModelType>();
    result &= static_cast<bool>(dtmc);
    result &= checkTask.getFormula().isInFragment(storm::logic::reachability()
                                                      .setRewardOperatorsAllowed(true)
                                                      .setReachabilityRewardFormulasAllowed(true)
                                                      .setBoundedUntilFormulasAllowed(true)
                                                      .setCumulativeRewardFormulasAllowed(true)
                                                      .setStepBoundedCumulativeRewardFormulasAllowed(true)
                                                      .setTimeBoundedCumulativeRewardFormulasAllowed(true)
                                                      .setTimeBoundedUntilFormulasAllowed(true)
                                                      .setStepBoundedUntilFormulasAllowed(true)
                                                      .setTimeBoundedUntilFormulasAllowed(true));
    return result;
}

template<typename SparseModelType, typename ConstantType>
void SparseRobustDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::specify(Environment const& env,
                                                                                          std::shared_ptr<storm::models::ModelBase> parametricModel,
                                                                                          CheckTask<storm::logic::Formula, ValueType> const& checkTask,
                                                                                          bool generateRegionSplitEstimates, bool allowModelSimplification) {
    auto dtmc = parametricModel->template as<SparseModelType>();
    specify_internal(env, dtmc, checkTask, generateRegionSplitEstimates, !allowModelSimplification);
}

template<typename SparseModelType, typename ConstantType>
void SparseRobustDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::specify_internal(Environment const& env,
                                                                                                   std::shared_ptr<SparseModelType> parametricModel,
                                                                                                   CheckTask<storm::logic::Formula, ValueType> const& checkTask,
                                                                                                   bool generateRegionSplitEstimates,
                                                                                                   bool skipModelSimplification) {
    STORM_LOG_ASSERT(this->canHandle(parametricModel, checkTask), "specified model and formula can not be handled by this.");

    reset();

    regionSplitEstimationsEnabled = generateRegionSplitEstimates;

    if (skipModelSimplification) {
        this->parametricModel = parametricModel;
        this->specifyFormula(env, checkTask);
    } else {
        auto simplifier = storm::transformer::SparseParametricDtmcSimplifier<SparseModelType>(*parametricModel);
        if (!simplifier.simplify(checkTask.getFormula())) {
            STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Simplifying the model was not successfull.");
        }
        this->parametricModel = simplifier.getSimplifiedModel();
        this->specifyFormula(env, checkTask.substituteFormula(*simplifier.getSimplifiedFormula()));
    }
}

template<typename SparseModelType, typename ConstantType>
void SparseRobustDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::specifyBoundedUntilFormula(
    const CheckTask<storm::logic::BoundedUntilFormula, ConstantType>& checkTask) {
    // TODO
    STORM_LOG_ERROR("Not implemented yet");
    // get the step bound
    STORM_LOG_THROW(!checkTask.getFormula().hasLowerBound(), storm::exceptions::NotSupportedException, "Lower step bounds are not supported.");
    STORM_LOG_THROW(checkTask.getFormula().hasUpperBound(), storm::exceptions::NotSupportedException, "Expected a bounded until formula with an upper bound.");
    STORM_LOG_THROW(checkTask.getFormula().getTimeBoundReference().isStepBound(), storm::exceptions::NotSupportedException,
                    "Expected a bounded until formula with step bounds.");
    stepBound = checkTask.getFormula().getUpperBound().evaluateAsInt();
    STORM_LOG_THROW(*stepBound > 0, storm::exceptions::NotSupportedException,
                    "Can not apply parameter lifting on step bounded formula: The step bound has to be positive.");
    if (checkTask.getFormula().isUpperBoundStrict()) {
        STORM_LOG_THROW(*stepBound > 0, storm::exceptions::NotSupportedException, "Expected a strict upper step bound that is greater than zero.");
        --(*stepBound);
    }
    STORM_LOG_THROW(*stepBound > 0, storm::exceptions::NotSupportedException,
                    "Can not apply parameter lifting on step bounded formula: The step bound has to be positive.");

    // get the results for the subformulas
    storm::modelchecker::SparsePropositionalModelChecker<SparseModelType> propositionalChecker(*this->parametricModel);
    STORM_LOG_THROW(propositionalChecker.canHandle(checkTask.getFormula().getLeftSubformula()) &&
                        propositionalChecker.canHandle(checkTask.getFormula().getRightSubformula()),
                    storm::exceptions::NotSupportedException, "Parameter lifting with non-propositional subformulas is not supported");
    storm::storage::BitVector phiStates =
        std::move(propositionalChecker.check(checkTask.getFormula().getLeftSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector());
    storm::storage::BitVector psiStates =
        std::move(propositionalChecker.check(checkTask.getFormula().getRightSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector());

    // get the maybeStates
    maybeStates = storm::utility::graph::performProbGreater0(this->parametricModel->getBackwardTransitions(), phiStates, psiStates, true, *stepBound);
    maybeStates &= ~psiStates;

    // set the result for all non-maybe states
    resultsForNonMaybeStates = std::vector<ConstantType>(this->parametricModel->getNumberOfStates(), storm::utility::zero<ConstantType>());
    storm::utility::vector::setVectorValues(resultsForNonMaybeStates, psiStates, storm::utility::one<ConstantType>());

    // if there are maybestates, create the parameterLifter
    if (!maybeStates.empty()) {
        // Create the vector of one-step probabilities to go to target states.
        std::vector<ValueType> b = this->parametricModel->getTransitionMatrix().getConstrainedRowSumVector(
            storm::storage::BitVector(this->parametricModel->getTransitionMatrix().getRowCount(), true), psiStates);
        parameterLifter = std::make_unique<storm::transformer::RobustParameterLifter<ValueType, ConstantType>>(this->parametricModel->getTransitionMatrix(), b,
                                                                                                               maybeStates, maybeStates, true);
    }

    // We know some bounds for the results so set them
    lowerResultBound = storm::utility::zero<ConstantType>();
    upperResultBound = storm::utility::one<ConstantType>();
    // No requirements for bounded formulas
    solverFactory->setRequirementsChecked(true);

    // For monotonicity checking
    std::pair<storm::storage::BitVector, storm::storage::BitVector> statesWithProbability01 =
        storm::utility::graph::performProb01(this->parametricModel->getBackwardTransitions(), phiStates, psiStates);
    this->orderExtender = storm::analysis::OrderExtender<ValueType, ConstantType>(&statesWithProbability01.second, &statesWithProbability01.first,
                                                                                  this->parametricModel->getTransitionMatrix());
}

template<typename SparseModelType, typename ConstantType>
void SparseRobustDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::specifyUntilFormula(
    Environment const& env, CheckTask<storm::logic::UntilFormula, ConstantType> const& checkTask) {
    // get the results for the subformulas
    storm::modelchecker::SparsePropositionalModelChecker<SparseModelType> propositionalChecker(*this->parametricModel);
    STORM_LOG_THROW(propositionalChecker.canHandle(checkTask.getFormula().getLeftSubformula()) &&
                        propositionalChecker.canHandle(checkTask.getFormula().getRightSubformula()),
                    storm::exceptions::NotSupportedException, "Parameter lifting with non-propositional subformulas is not supported");
    storm::storage::BitVector phiStates =
        std::move(propositionalChecker.check(checkTask.getFormula().getLeftSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector());
    storm::storage::BitVector psiStates =
        std::move(propositionalChecker.check(checkTask.getFormula().getRightSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector());

    // get the maybeStates
    std::pair<storm::storage::BitVector, storm::storage::BitVector> statesWithProbability01 =
        storm::utility::graph::performProb01(this->parametricModel->getBackwardTransitions(), phiStates, psiStates);

    maybeStates = ~(statesWithProbability01.first | statesWithProbability01.second);

    // set the result for all non-maybe states
    resultsForNonMaybeStates = std::vector<ConstantType>(this->parametricModel->getNumberOfStates(), storm::utility::zero<ConstantType>());
    storm::utility::vector::setVectorValues(resultsForNonMaybeStates, statesWithProbability01.second, storm::utility::one<ConstantType>());

    // if there are maybestates, create the parameterLifter
    if (!maybeStates.empty()) {
        // Create the vector of one-step probabilities to go to target states.
        // See SparseMdpPrctlHelper.cpp function "computeFixedPointSystemUntilProbabilities"
        std::vector<ValueType> target(this->parametricModel->getNumberOfStates(), storm::utility::zero<ValueType>());
        storm::utility::vector::setVectorValues(target, statesWithProbability01.second, storm::utility::one<ValueType>());
        auto rowFilter = this->parametricModel->getTransitionMatrix().getRowFilter(maybeStates);
        auto filteredMatrix = this->parametricModel->getTransitionMatrix().filterEntries(rowFilter);
        storm::storage::BitVector allTrue(maybeStates.size(), true);
        parameterLifter = std::make_unique<storm::transformer::RobustParameterLifter<ValueType, ConstantType>>(filteredMatrix, target, allTrue, allTrue, true);
    }

    // We know some bounds for the results so set them
    lowerResultBound = storm::utility::zero<ConstantType>();
    upperResultBound = storm::utility::one<ConstantType>();

    // The solution of the min-max equation system will always be unique (assuming graph-preserving instantiations, every induced DTMC has the same graph
    // structure).
    auto req = solverFactory->getRequirements(env, true, true, boost::none, false);
    req.requireValidInitialScheduler(false);
    req.clearBounds();
    STORM_LOG_THROW(!req.hasEnabledCriticalRequirement(), storm::exceptions::UncheckedRequirementException,
                    "Solver requirements " + req.getEnabledRequirementsAsString() + " not checked.");
    solverFactory->setRequirementsChecked(true);

    this->orderExtender = storm::analysis::OrderExtender<ValueType, ConstantType>(&statesWithProbability01.second, &statesWithProbability01.first,
                                                                                  this->parametricModel->getTransitionMatrix());
}

template<typename SparseModelType, typename ConstantType>
void SparseRobustDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::specifyReachabilityRewardFormula(
    Environment const& env, CheckTask<storm::logic::EventuallyFormula, ConstantType> const& checkTask) {
    // get the results for the subformula
    storm::modelchecker::SparsePropositionalModelChecker<SparseModelType> propositionalChecker(*this->parametricModel);
    STORM_LOG_THROW(propositionalChecker.canHandle(checkTask.getFormula().getSubformula()), storm::exceptions::NotSupportedException,
                    "Parameter lifting with non-propositional subformulas is not supported");
    storm::storage::BitVector targetStates =
        std::move(propositionalChecker.check(checkTask.getFormula().getSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector());
    // get the maybeStates
    storm::storage::BitVector infinityStates = storm::utility::graph::performProb1(
        this->parametricModel->getBackwardTransitions(), storm::storage::BitVector(this->parametricModel->getNumberOfStates(), true), targetStates);
    infinityStates.complement();
    maybeStates = ~(targetStates | infinityStates);

    // set the result for all the non-maybe states
    resultsForNonMaybeStates = std::vector<ConstantType>(this->parametricModel->getNumberOfStates(), storm::utility::zero<ConstantType>());
    storm::utility::vector::setVectorValues(resultsForNonMaybeStates, infinityStates, storm::utility::infinity<ConstantType>());

    // if there are maybestates, create the parameterLifter
    if (!maybeStates.empty()) {
        // Create the vector of one-step probabilities to go to target states.
        // See SparseMdpPrctlHelper.cpp function "computeFixedPointSystemUntilProbabilities"

        // Create the reward vector
        STORM_LOG_THROW((checkTask.isRewardModelSet() && this->parametricModel->hasRewardModel(checkTask.getRewardModel())) ||
                            (!checkTask.isRewardModelSet() && this->parametricModel->hasUniqueRewardModel()),
                        storm::exceptions::InvalidPropertyException, "The reward model specified by the CheckTask is not available in the given model.");

        typename SparseModelType::RewardModelType const& rewardModel =
            checkTask.isRewardModelSet() ? this->parametricModel->getRewardModel(checkTask.getRewardModel()) : this->parametricModel->getUniqueRewardModel();

        std::vector<ValueType> b = rewardModel.getTotalRewardVector(this->parametricModel->getTransitionMatrix());

        auto rowFilter = this->parametricModel->getTransitionMatrix().getRowFilter(maybeStates);
        auto filteredMatrix = this->parametricModel->getTransitionMatrix().filterEntries(rowFilter);
        storm::storage::BitVector allTrue(maybeStates.size(), true);
        parameterLifter = std::make_unique<storm::transformer::RobustParameterLifter<ValueType, ConstantType>>(filteredMatrix, b, allTrue, allTrue, true);
    }

    // We only know a lower bound for the result
    lowerResultBound = storm::utility::zero<ConstantType>();

    // The solution of the min-max equation system will always be unique (assuming graph-preserving instantiations, every induced DTMC has the same graph
    // structure).
    auto req = solverFactory->getRequirements(env, true, true, boost::none, false);
    req.clearLowerBounds();
    if (req.upperBounds()) {
        solvingRequiresUpperRewardBounds = true;
        req.clearUpperBounds();
    }
    STORM_LOG_THROW(!req.hasEnabledCriticalRequirement(), storm::exceptions::UncheckedRequirementException,
                    "Solver requirements " + req.getEnabledRequirementsAsString() + " not checked.");
    solverFactory->setRequirementsChecked(true);
}

template<typename SparseModelType, typename ConstantType>
void SparseRobustDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::specifyCumulativeRewardFormula(
    const CheckTask<storm::logic::CumulativeRewardFormula, ConstantType>& checkTask) {
    // TODO
    STORM_LOG_ERROR("Not implemented yet");
    // Obtain the stepBound
    stepBound = checkTask.getFormula().getBound().evaluateAsInt();
    if (checkTask.getFormula().isBoundStrict()) {
        STORM_LOG_THROW(*stepBound > 0, storm::exceptions::NotSupportedException, "Expected a strict upper step bound that is greater than zero.");
        --(*stepBound);
    }
    STORM_LOG_THROW(*stepBound > 0, storm::exceptions::NotSupportedException,
                    "Can not apply parameter lifting on step bounded formula: The step bound has to be positive.");

    // Every state is a maybeState
    maybeStates = storm::storage::BitVector(this->parametricModel->getTransitionMatrix().getColumnCount(), true);
    resultsForNonMaybeStates = std::vector<ConstantType>(this->parametricModel->getNumberOfStates());

    // Create the reward vector
    STORM_LOG_THROW((checkTask.isRewardModelSet() && this->parametricModel->hasRewardModel(checkTask.getRewardModel())) ||
                        (!checkTask.isRewardModelSet() && this->parametricModel->hasUniqueRewardModel()),
                    storm::exceptions::InvalidPropertyException, "The reward model specified by the CheckTask is not available in the given model.");
    typename SparseModelType::RewardModelType const& rewardModel =
        checkTask.isRewardModelSet() ? this->parametricModel->getRewardModel(checkTask.getRewardModel()) : this->parametricModel->getUniqueRewardModel();
    std::vector<ValueType> b = rewardModel.getTotalRewardVector(this->parametricModel->getTransitionMatrix());

    parameterLifter = std::make_unique<storm::transformer::RobustParameterLifter<ValueType, ConstantType>>(this->parametricModel->getTransitionMatrix(), b,
                                                                                                           maybeStates, maybeStates, true);

    // We only know a lower bound for the result
    lowerResultBound = storm::utility::zero<ConstantType>();

    // No requirements for bounded reward formula
    solverFactory->setRequirementsChecked(true);
}

template<typename SparseModelType, typename ConstantType>
storm::modelchecker::SparseInstantiationModelChecker<SparseModelType, ConstantType>&
SparseRobustDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::getInstantiationCheckerSAT() {
    if (!instantiationCheckerSAT) {
        instantiationCheckerSAT =
            std::make_unique<storm::modelchecker::SparseDtmcInstantiationModelChecker<SparseModelType, ConstantType>>(*this->parametricModel);
        instantiationCheckerSAT->specifyFormula(this->currentCheckTask->template convertValueType<ValueType>());
        instantiationCheckerSAT->setInstantiationsAreGraphPreserving(true);
    }
    return *instantiationCheckerSAT;
}

template<typename SparseModelType, typename ConstantType>
storm::modelchecker::SparseInstantiationModelChecker<SparseModelType, ConstantType>&
SparseRobustDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::getInstantiationCheckerVIO() {
    if (!instantiationCheckerVIO) {
        instantiationCheckerVIO =
            std::make_unique<storm::modelchecker::SparseDtmcInstantiationModelChecker<SparseModelType, ConstantType>>(*this->parametricModel);
        instantiationCheckerVIO->specifyFormula(this->currentCheckTask->template convertValueType<ValueType>());
        instantiationCheckerVIO->setInstantiationsAreGraphPreserving(true);
    }
    return *instantiationCheckerVIO;
}

template<typename SparseModelType, typename ConstantType>
storm::modelchecker::SparseInstantiationModelChecker<SparseModelType, ConstantType>&
SparseRobustDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::getInstantiationChecker() {
    if (!instantiationChecker) {
        instantiationChecker =
            std::make_unique<storm::modelchecker::SparseDtmcInstantiationModelChecker<SparseModelType, ConstantType>>(*this->parametricModel);
        instantiationChecker->specifyFormula(this->currentCheckTask->template convertValueType<ValueType>());
        instantiationChecker->setInstantiationsAreGraphPreserving(true);
    }
    return *instantiationChecker;
}

template<typename SparseModelType, typename ConstantType>
std::unique_ptr<CheckResult> SparseRobustDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::computeQuantitativeValues(
    Environment const& env, storm::storage::ParameterRegion<ValueType> const& region, storm::solver::OptimizationDirection const& dirForParameters,
    std::shared_ptr<storm::analysis::LocalMonotonicityResult<VariableType>> localMonotonicityResult) {
    if (maybeStates.empty()) {
        return std::make_unique<storm::modelchecker::ExplicitQuantitativeCheckResult<ConstantType>>(resultsForNonMaybeStates);
    }
    parameterLifter->specifyRegion(region, dirForParameters);

    if (stepBound) {
        // assert(*stepBound > 0);
        // x = std::vector<ConstantType>(maybeStates.getNumberOfSetBits(), storm::utility::zero<ConstantType>());
        // auto multiplier = storm::solver::MultiplierFactory<Interval>().create(env, parameterLifter->getMatrix());
        // multiplier->repeatedMultiplyAndReduce(env, dirForParameters, x, &parameterLifter->getVector(), *stepBound);
    } else {
        auto solver = solverFactory->create(env, parameterLifter->getMatrix());
        solver->setHasUniqueSolution();
        solver->setHasNoEndComponents();
        if (lowerResultBound)
            solver->setLowerBound(lowerResultBound.get());
        if (upperResultBound) {
            solver->setUpperBound(upperResultBound.get());
        } else if (solvingRequiresUpperRewardBounds) {
            // // For the min-case, we use DS-MPI, for the max-case variant 2 of the Baier et al. paper (CAV'17).
            // std::vector<ConstantType> oneStepProbs;
            // oneStepProbs.reserve(parameterLifter->getMatrix().getRowCount());
            // for (uint64_t row = 0; row < parameterLifter->getMatrix().getRowCount(); ++row) {
            //     oneStepProbs.push_back(storm::utility::one<ConstantType>() - parameterLifter->getMatrix().getRowSum(row));
            // }
            // if (dirForParameters == storm::OptimizationDirection::Minimize) {
            //     storm::modelchecker::helper::DsMpiMdpUpperRewardBoundsComputer<ConstantType> dsmpi(parameterLifter->getMatrix(),
            //     parameterLifter->getVector(),
            //                                                                                        oneStepProbs);
            //     solver->setUpperBounds(dsmpi.computeUpperBounds());
            // } else {
            //     storm::modelchecker::helper::BaierUpperRewardBoundsComputer<ConstantType> baier(parameterLifter->getMatrix(), parameterLifter->getVector(),
            //                                                                                     oneStepProbs);
            //     solver->setUpperBound(baier.computeUpperBound());
            // }
        }
        solver->setTrackScheduler(true);

        if (storm::solver::minimize(dirForParameters) && minSchedChoices)
            solver->setInitialScheduler(std::move(minSchedChoices.get()));
        if (storm::solver::maximize(dirForParameters) && maxSchedChoices)
            solver->setInitialScheduler(std::move(maxSchedChoices.get()));

        if (this->currentCheckTask->isBoundSet() && solver->hasInitialScheduler()) {
            // If we reach this point, we know that after applying the hint, the x-values can only become larger (if we maximize) or smaller (if we minimize).
            std::unique_ptr<storm::solver::TerminationCondition<ConstantType>> termCond;
            storm::storage::BitVector relevantStatesInSubsystem = this->currentCheckTask->isOnlyInitialStatesRelevantSet()
                                                                      ? this->parametricModel->getInitialStates() % maybeStates
                                                                      : storm::storage::BitVector(maybeStates.getNumberOfSetBits(), true);
            if (storm::solver::minimize(dirForParameters)) {
                // Terminate if the value for ALL relevant states is already below the threshold
                termCond = std::make_unique<storm::solver::TerminateIfFilteredExtremumBelowThreshold<ConstantType>>(
                    relevantStatesInSubsystem, true, this->currentCheckTask->getBoundThreshold(), false);
            } else {
                // Terminate if the value for ALL relevant states is already above the threshold
                termCond = std::make_unique<storm::solver::TerminateIfFilteredExtremumExceedsThreshold<ConstantType>>(
                    relevantStatesInSubsystem, true, this->currentCheckTask->getBoundThreshold(), true);
            }
            solver->setTerminationCondition(std::move(termCond));
        }

        // Invoke the solver
        x.resize(parameterLifter->getVector().size(), storm::utility::zero<ConstantType>());
        solver->solveEquations(env, dirForParameters, x, parameterLifter->getVector());
        // if (storm::solver::minimize(dirForParameters)) {
        //     minSchedChoices = solver->getSchedulerChoices();
        // } else {
        //     maxSchedChoices = solver->getSchedulerChoices();
        // }
    }

    // Get the result for the complete model (including maybestates)
    std::vector<ConstantType> result = resultsForNonMaybeStates;
    auto maybeStateResIt = x.begin();
    for (auto const& maybeState : maybeStates) {
        result[maybeState] = *maybeStateResIt;
        ++maybeStateResIt;
    }
    return std::make_unique<storm::modelchecker::ExplicitQuantitativeCheckResult<ConstantType>>(std::move(result));
}

// template<typename SparseModelType, typename ConstantType>
// void SparseRobustDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::computeRegionSplitEstimates(
//     std::vector<ConstantType> const& quantitativeResult, std::vector<uint_fast64_t> const& schedulerChoices,
//     storm::storage::ParameterRegion<ValueType> const& region, storm::solver::OptimizationDirection const& dirForParameters) {
//     std::map<VariableType, double> deltaLower, deltaUpper;
//     for (auto const& p : region.getVariables()) {
//         deltaLower.insert(std::make_pair(p, 0.0));
//         deltaUpper.insert(std::make_pair(p, 0.0));
//     }
//     auto const& choiceValuations = parameterLifter->getRowLabels();
//     auto const& matrix = parameterLifter->getMatrix();
//     auto const& vector = parameterLifter->getVector();

//     std::vector<ConstantType> stateResults;
//     for (uint64_t state = 0; state < schedulerChoices.size(); ++state) {
//         uint64_t rowOffset = matrix.getRowGroupIndices()[state];
//         uint64_t optimalChoice = schedulerChoices[state];
//         auto const& optimalChoiceVal = choiceValuations[rowOffset + optimalChoice];
//         assert(optimalChoiceVal.getUnspecifiedParameters().empty());
//         stateResults.clear();
//         for (uint64_t row = rowOffset; row < matrix.getRowGroupIndices()[state + 1]; ++row) {
//             stateResults.push_back(matrix.multiplyRowWithVector(row, quantitativeResult) + vector[row]);
//         }
//         // Do this twice, once for upperbound once for lowerbound
//         bool checkUpperParameters = false;
//         do {
//             auto const& consideredParameters = checkUpperParameters ? optimalChoiceVal.getUpperParameters() : optimalChoiceVal.getLowerParameters();
//             for (auto const& p : consideredParameters) {
//                 // Find the 'best' choice that assigns the parameter to the other bound
//                 ConstantType bestValue = 0;
//                 bool foundBestValue = false;
//                 for (uint64_t choice = 0; choice < stateResults.size(); ++choice) {
//                     if (choice != optimalChoice) {
//                         auto const& otherBoundParsOfChoice = checkUpperParameters ? choiceValuations[rowOffset + choice].getLowerParameters()
//                                                                                   : choiceValuations[rowOffset + choice].getUpperParameters();
//                         if (otherBoundParsOfChoice.find(p) != otherBoundParsOfChoice.end()) {
//                             ConstantType const& choiceValue = stateResults[choice];
//                             if (!foundBestValue || (storm::solver::minimize(dirForParameters) ? choiceValue < bestValue : choiceValue > bestValue)) {
//                                 foundBestValue = true;
//                                 bestValue = choiceValue;
//                             }
//                         }
//                     }
//                 }
//                 auto optimal = storm::utility::convertNumber<double>(stateResults[optimalChoice]);
//                 auto diff = optimal - storm::utility::convertNumber<double>(bestValue);
//                 if (foundBestValue) {
//                     if (checkUpperParameters) {
//                         deltaLower[p] += std::abs(diff);
//                     } else {
//                         deltaUpper[p] += std::abs(diff);
//                     }
//                 }
//             }
//             checkUpperParameters = !checkUpperParameters;
//         } while (checkUpperParameters);
//     }

//     regionSplitEstimates.clear();
//     useRegionSplitEstimates = false;
//     for (auto const& p : region.getVariables()) {
//         if (this->possibleMonotoneParameters.find(p) != this->possibleMonotoneParameters.end()) {
//             if (deltaLower[p] > deltaUpper[p] && deltaUpper[p] >= 0.0001) {
//                 regionSplitEstimates.insert(std::make_pair(p, deltaUpper[p]));
//                 useRegionSplitEstimates = true;
//             } else if (deltaLower[p] <= deltaUpper[p] && deltaLower[p] >= 0.0001) {
//                 {
//                     regionSplitEstimates.insert(std::make_pair(p, deltaLower[p]));
//                     useRegionSplitEstimates = true;
//                 }
//             }
//         }
//     }
//     // large regionsplitestimate implies that parameter p occurs as p and 1-p at least once
// }

template<typename SparseModelType, typename ConstantType>
void SparseRobustDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::reset() {
    maybeStates.resize(0);
    resultsForNonMaybeStates.clear();
    stepBound = boost::none;
    instantiationChecker = nullptr;
    parameterLifter = nullptr;
    minSchedChoices = boost::none;
    maxSchedChoices = boost::none;
    x.clear();
    lowerResultBound = boost::none;
    upperResultBound = boost::none;
    regionSplitEstimationsEnabled = false;
}

template<typename SparseModelType, typename ConstantType>
boost::optional<storm::storage::Scheduler<ConstantType>> SparseRobustDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::getCurrentMinScheduler() {
    if (!minSchedChoices) {
        return boost::none;
    }

    storm::storage::Scheduler<ConstantType> result(minSchedChoices->size());
    uint_fast64_t state = 0;
    for (auto const& schedulerChoice : minSchedChoices.get()) {
        result.setChoice(schedulerChoice, state);
        ++state;
    }

    return result;
}

template<typename SparseModelType, typename ConstantType>
boost::optional<storm::storage::Scheduler<ConstantType>> SparseRobustDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::getCurrentMaxScheduler() {
    if (!maxSchedChoices) {
        return boost::none;
    }

    storm::storage::Scheduler<ConstantType> result(maxSchedChoices->size());
    uint_fast64_t state = 0;
    for (auto const& schedulerChoice : maxSchedChoices.get()) {
        result.setChoice(schedulerChoice, state);
        ++state;
    }

    return result;
}

template<typename SparseModelType, typename ConstantType>
bool SparseRobustDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::isRegionSplitEstimateSupported() const {
    return false;
}

template<typename SparseModelType, typename ConstantType>
std::map<typename RegionModelChecker<typename SparseModelType::ValueType>::VariableType, double>
SparseRobustDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::getRegionSplitEstimate() const {
    STORM_LOG_THROW(isRegionSplitEstimateSupported(), storm::exceptions::InvalidOperationException,
                    "Region split estimation requested but are not enabled (or supported).");
    return regionSplitEstimates;
}
template<typename SparseModelType, typename ConstantType>
void SparseRobustDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::splitSmart(
    storm::storage::ParameterRegion<ValueType>& region, std::vector<storm::storage::ParameterRegion<ValueType>>& regionVector,
    storm::analysis::MonotonicityResult<VariableType>& monRes, bool splitForExtremum) const {
    assert(regionVector.size() == 0);

    std::multimap<double, VariableType> sortedOnValues;
    std::set<VariableType> consideredVariables;
    if (splitForExtremum) {
        if (regionSplitEstimationsEnabled && useRegionSplitEstimates) {
            STORM_LOG_INFO("Splitting based on region split estimates");
            for (auto& entry : regionSplitEstimates) {
                assert(!this->isUseMonotonicitySet() ||
                       (!monRes.isMonotone(entry.first) && this->possibleMonotoneParameters.find(entry.first) != this->possibleMonotoneParameters.end()));
                //                            sortedOnValues.insert({-(entry.second * storm::utility::convertNumber<double>(region.getDifference(entry.first))*
                //                            storm::utility::convertNumber<double>(region.getDifference(entry.first))), entry.first});
                sortedOnValues.insert({-(entry.second), entry.first});
            }

            for (auto itr = sortedOnValues.begin(); itr != sortedOnValues.end() && consideredVariables.size() < maxSplitDimensions; ++itr) {
                consideredVariables.insert(itr->second);
            }
            assert(consideredVariables.size() > 0);
            region.split(region.getCenterPoint(), regionVector, std::move(consideredVariables));
        } else {
            STORM_LOG_INFO("Splitting based on sorting");

            auto& sortedOnDifference = region.getVariablesSorted();
            for (auto itr = sortedOnDifference.begin(); itr != sortedOnDifference.end() && consideredVariables.size() < maxSplitDimensions; ++itr) {
                if (!this->isUseMonotonicitySet() || !monRes.isMonotone(itr->second)) {
                    consideredVariables.insert(itr->second);
                }
            }
            assert(consideredVariables.size() > 0 || (monRes.isDone() && monRes.isAllMonotonicity()));
            region.split(region.getCenterPoint(), regionVector, std::move(consideredVariables));
        }
    } else {
        // split for pla
        if (regionSplitEstimationsEnabled && useRegionSplitEstimates) {
            STORM_LOG_INFO("Splitting based on region split estimates");
            ConstantType diff = this->lastValue - (this->currentCheckTask->getFormula().asOperatorFormula().template getThresholdAs<ConstantType>());
            for (auto& entry : regionSplitEstimates) {
                if ((!this->isUseMonotonicitySet() || !monRes.isMonotone(entry.first)) && storm::utility::convertNumber<ConstantType>(entry.second) > diff) {
                    sortedOnValues.insert({-(entry.second * storm::utility::convertNumber<double>(region.getDifference(entry.first)) *
                                             storm::utility::convertNumber<double>(region.getDifference(entry.first))),
                                           entry.first});
                }
            }

            for (auto itr = sortedOnValues.begin(); itr != sortedOnValues.end() && consideredVariables.size() < maxSplitDimensions; ++itr) {
                consideredVariables.insert(itr->second);
            }
        }
        if (consideredVariables.size() == 0) {
            auto& sortedOnDifference = region.getVariablesSorted();
            for (auto itr = sortedOnDifference.begin(); itr != sortedOnDifference.end() && consideredVariables.size() < maxSplitDimensions; ++itr) {
                consideredVariables.insert(itr->second);
            }
        }
        assert(consideredVariables.size() > 0);
        region.split(region.getCenterPoint(), regionVector, std::move(consideredVariables));
    }
}

template<typename SparseModelType, typename ConstantType>
void SparseRobustDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::setMaxSplitDimensions(uint64_t newValue) {
    maxSplitDimensions = newValue;
}

template<typename SparseModelType, typename ConstantType>
void SparseRobustDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::resetMaxSplitDimensions() {
    maxSplitDimensions = std::numeric_limits<uint64_t>::max();
}

template class SparseRobustDtmcParameterLiftingModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double>;
}  // namespace modelchecker
}  // namespace storm
