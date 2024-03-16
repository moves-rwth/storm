#include "storm-pars/modelchecker/region/SparseDtmcParameterLiftingModelChecker.h"

#include "storm-pars/modelchecker/region/monotonicity/OrderBasedMonotonicityBackend.h"
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

namespace storm {
namespace modelchecker {

template<typename SparseModelType, typename ConstantType>
SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::SparseDtmcParameterLiftingModelChecker()
    : SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>(
          std::make_unique<storm::solver::GeneralMinMaxLinearEquationSolverFactory<ConstantType>>()) {
    // Intentionally left empty
}

template<typename SparseModelType, typename ConstantType>
SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::SparseDtmcParameterLiftingModelChecker(
    std::unique_ptr<storm::solver::MinMaxLinearEquationSolverFactory<ConstantType>>&& solverFactory)
    : solverFactory(std::move(solverFactory)), solvingRequiresUpperRewardBounds(false) {
    // Intentionally left empty
}

template<typename SparseModelType, typename ConstantType>
bool SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::canHandle(std::shared_ptr<storm::models::ModelBase> parametricModel,
                                                                                      CheckTask<storm::logic::Formula, ParametricType> const& checkTask) const {
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
void SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::specify(Environment const& env,
                                                                                    std::shared_ptr<storm::models::ModelBase> parametricModel,
                                                                                    CheckTask<storm::logic::Formula, ParametricType> const& checkTask,
                                                                                    std::optional<detail::RegionSplitEstimateKind> generateRegionSplitEstimates,
                                                                                    std::shared_ptr<MonotonicityBackend<ParametricType>> monotonicityBackend,
                                                                                    bool allowModelSimplifications) {
    STORM_LOG_ASSERT(this->canHandle(parametricModel, checkTask), "specified model and formula can not be handled by this.");
    this->specifySplitEstimates(generateRegionSplitEstimates, checkTask);
    this->specifyMonotonicity(monotonicityBackend, checkTask);
    auto dtmc = parametricModel->template as<SparseModelType>();
    if (isOrderBasedMonotonicityBackend()) {
        getOrderBasedMonotonicityBackend().initializeMonotonicityChecker(dtmc->getTransitionMatrix());
    }

    reset();

    if (allowModelSimplifications) {
        auto simplifier = storm::transformer::SparseParametricDtmcSimplifier<SparseModelType>(*dtmc);
        if (!simplifier.simplify(checkTask.getFormula())) {
            STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Simplifying the model was not successfull.");
        }
        this->parametricModel = simplifier.getSimplifiedModel();
        this->specifyFormula(env, checkTask.substituteFormula(*simplifier.getSimplifiedFormula()));
    } else {
        this->parametricModel = dtmc;
        this->specifyFormula(env, checkTask);
    }
}

template<typename SparseModelType, typename ConstantType>
void SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::specifyBoundedUntilFormula(
    const CheckTask<storm::logic::BoundedUntilFormula, ConstantType>& checkTask) {
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
        std::vector<ParametricType> b = this->parametricModel->getTransitionMatrix().getConstrainedRowSumVector(
            storm::storage::BitVector(this->parametricModel->getTransitionMatrix().getRowCount(), true), psiStates);
        parameterLifter = std::make_unique<storm::transformer::ParameterLifter<ParametricType, ConstantType>>(
            this->parametricModel->getTransitionMatrix(), b, maybeStates, maybeStates, false, isOrderBasedMonotonicityBackend());
    }

    // We know some bounds for the results so set them
    lowerResultBound = storm::utility::zero<ConstantType>();
    upperResultBound = storm::utility::one<ConstantType>();
    // No requirements for bounded formulas
    solverFactory->setRequirementsChecked(true);

    if (isOrderBasedMonotonicityBackend()) {
        auto [prob0, prob1] = storm::utility::graph::performProb01(this->parametricModel->getBackwardTransitions(), phiStates, psiStates);
        getOrderBasedMonotonicityBackend().initializeOrderExtender(prob1, prob0, this->parametricModel->getTransitionMatrix());
    }
}

template<typename SparseModelType, typename ConstantType>
void SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::specifyUntilFormula(
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
        std::vector<ParametricType> b = this->parametricModel->getTransitionMatrix().getConstrainedRowSumVector(
            storm::storage::BitVector(this->parametricModel->getTransitionMatrix().getRowCount(), true), statesWithProbability01.second);
        parameterLifter = std::make_unique<storm::transformer::ParameterLifter<ParametricType, ConstantType>>(
            this->parametricModel->getTransitionMatrix(), b, maybeStates, maybeStates, isValueDeltaRegionSplitEstimates(), isOrderBasedMonotonicityBackend());
    }

    // We know some bounds for the results so set them
    lowerResultBound = storm::utility::zero<ConstantType>();
    upperResultBound = storm::utility::one<ConstantType>();

    // The solution of the min-max equation system will always be unique (assuming graph-preserving instantiations, every induced DTMC has the same graph
    // structure).
    auto req = solverFactory->getRequirements(env, true, true, boost::none, true);
    req.clearBounds();
    STORM_LOG_THROW(!req.hasEnabledCriticalRequirement(), storm::exceptions::UncheckedRequirementException,
                    "Solver requirements " + req.getEnabledRequirementsAsString() + " not checked.");
    solverFactory->setRequirementsChecked(true);

    if (isOrderBasedMonotonicityBackend()) {
        getOrderBasedMonotonicityBackend().initializeOrderExtender(statesWithProbability01.second, statesWithProbability01.first,
                                                                   this->parametricModel->getTransitionMatrix());
    }
}

template<typename SparseModelType, typename ConstantType>
void SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::specifyReachabilityRewardFormula(
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
        // Create the reward vector
        STORM_LOG_THROW((checkTask.isRewardModelSet() && this->parametricModel->hasRewardModel(checkTask.getRewardModel())) ||
                            (!checkTask.isRewardModelSet() && this->parametricModel->hasUniqueRewardModel()),
                        storm::exceptions::InvalidPropertyException, "The reward model specified by the CheckTask is not available in the given model.");

        typename SparseModelType::RewardModelType const& rewardModel =
            checkTask.isRewardModelSet() ? this->parametricModel->getRewardModel(checkTask.getRewardModel()) : this->parametricModel->getUniqueRewardModel();

        std::vector<ParametricType> b = rewardModel.getTotalRewardVector(this->parametricModel->getTransitionMatrix());

        parameterLifter = std::make_unique<storm::transformer::ParameterLifter<ParametricType, ConstantType>>(
            this->parametricModel->getTransitionMatrix(), b, maybeStates, maybeStates, isValueDeltaRegionSplitEstimates());
    }

    // We only know a lower bound for the result
    lowerResultBound = storm::utility::zero<ConstantType>();

    // The solution of the min-max equation system will always be unique (assuming graph-preserving instantiations, every induced DTMC has the same graph
    // structure).
    auto req = solverFactory->getRequirements(env, true, true, boost::none, true);
    req.clearLowerBounds();
    if (req.upperBounds()) {
        solvingRequiresUpperRewardBounds = true;
        req.clearUpperBounds();
    }
    STORM_LOG_THROW(!req.hasEnabledCriticalRequirement(), storm::exceptions::UncheckedRequirementException,
                    "Solver requirements " + req.getEnabledRequirementsAsString() + " not checked.");
    solverFactory->setRequirementsChecked(true);
    STORM_LOG_WARN_COND(!isOrderBasedMonotonicityBackend(), "Order-based monotonicity not used for reachability reward formula.");
}

template<typename SparseModelType, typename ConstantType>
void SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::specifyCumulativeRewardFormula(
    const CheckTask<storm::logic::CumulativeRewardFormula, ConstantType>& checkTask) {
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
    std::vector<ParametricType> b = rewardModel.getTotalRewardVector(this->parametricModel->getTransitionMatrix());

    parameterLifter = std::make_unique<storm::transformer::ParameterLifter<ParametricType, ConstantType>>(this->parametricModel->getTransitionMatrix(), b,
                                                                                                          maybeStates, maybeStates);

    // We only know a lower bound for the result
    lowerResultBound = storm::utility::zero<ConstantType>();

    // No requirements for bounded reward formula
    solverFactory->setRequirementsChecked(true);

    STORM_LOG_WARN_COND(!isOrderBasedMonotonicityBackend(), "Order-based monotonicity not used for cumulative reward formula.");
}

template<typename SparseModelType, typename ConstantType>
storm::modelchecker::SparseInstantiationModelChecker<SparseModelType, ConstantType>&
SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::getInstantiationCheckerSAT() {
    if (!instantiationCheckerSAT) {
        instantiationCheckerSAT =
            std::make_unique<storm::modelchecker::SparseDtmcInstantiationModelChecker<SparseModelType, ConstantType>>(*this->parametricModel);
        instantiationCheckerSAT->specifyFormula(this->currentCheckTask->template convertValueType<ParametricType>());
        instantiationCheckerSAT->setInstantiationsAreGraphPreserving(true);
    }
    return *instantiationCheckerSAT;
}

template<typename SparseModelType, typename ConstantType>
storm::modelchecker::SparseInstantiationModelChecker<SparseModelType, ConstantType>&
SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::getInstantiationCheckerVIO() {
    if (!instantiationCheckerVIO) {
        instantiationCheckerVIO =
            std::make_unique<storm::modelchecker::SparseDtmcInstantiationModelChecker<SparseModelType, ConstantType>>(*this->parametricModel);
        instantiationCheckerVIO->specifyFormula(this->currentCheckTask->template convertValueType<ParametricType>());
        instantiationCheckerVIO->setInstantiationsAreGraphPreserving(true);
    }
    return *instantiationCheckerVIO;
}

template<typename SparseModelType, typename ConstantType>
storm::modelchecker::SparseInstantiationModelChecker<SparseModelType, ConstantType>&
SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::getInstantiationChecker() {
    if (!instantiationChecker) {
        instantiationChecker =
            std::make_unique<storm::modelchecker::SparseDtmcInstantiationModelChecker<SparseModelType, ConstantType>>(*this->parametricModel);
        instantiationChecker->specifyFormula(this->currentCheckTask->template convertValueType<ParametricType>());
        instantiationChecker->setInstantiationsAreGraphPreserving(true);
    }
    return *instantiationChecker;
}

template<typename SparseModelType, typename ConstantType>
std::unique_ptr<CheckResult> SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::computeQuantitativeValues(
    Environment const& env, detail::AnnotatedRegion<ParametricType>& region, storm::solver::OptimizationDirection const& dirForParameters) {
    if (maybeStates.empty()) {
        return std::make_unique<storm::modelchecker::ExplicitQuantitativeCheckResult<ConstantType>>(resultsForNonMaybeStates);
    }
    parameterLifter->specifyRegion(region.region, dirForParameters);

    if (stepBound) {
        assert(*stepBound > 0);
        x = std::vector<ConstantType>(maybeStates.getNumberOfSetBits(), storm::utility::zero<ConstantType>());
        auto multiplier = storm::solver::MultiplierFactory<ConstantType>().create(env, parameterLifter->getMatrix());
        multiplier->repeatedMultiplyAndReduce(env, dirForParameters, x, &parameterLifter->getVector(), *stepBound);
    } else {
        auto solver = solverFactory->create(env, parameterLifter->getMatrix());
        solver->setHasUniqueSolution();
        solver->setHasNoEndComponents();
        if (lowerResultBound)
            solver->setLowerBound(lowerResultBound.value());
        if (upperResultBound) {
            solver->setUpperBound(upperResultBound.value());
        } else if (solvingRequiresUpperRewardBounds) {
            // For the min-case, we use DS-MPI, for the max-case variant 2 of the Baier et al. paper (CAV'17).
            std::vector<ConstantType> oneStepProbs;
            oneStepProbs.reserve(parameterLifter->getMatrix().getRowCount());
            for (uint64_t row = 0; row < parameterLifter->getMatrix().getRowCount(); ++row) {
                oneStepProbs.push_back(storm::utility::one<ConstantType>() - parameterLifter->getMatrix().getRowSum(row));
            }
            if (dirForParameters == storm::OptimizationDirection::Minimize) {
                storm::modelchecker::helper::DsMpiMdpUpperRewardBoundsComputer<ConstantType> dsmpi(parameterLifter->getMatrix(), parameterLifter->getVector(),
                                                                                                   oneStepProbs);
                solver->setUpperBounds(dsmpi.computeUpperBounds());
            } else {
                storm::modelchecker::helper::BaierUpperRewardBoundsComputer<ConstantType> baier(parameterLifter->getMatrix(), parameterLifter->getVector(),
                                                                                                oneStepProbs);
                solver->setUpperBound(baier.computeUpperBound());
            }
        }
        solver->setTrackScheduler(true);

        // Get reference to relevant scheduler choices
        auto& choices = storm::solver::minimize(dirForParameters) ? minSchedChoices : maxSchedChoices;

        // Potentially fix some choices if monotonicity is known
        storm::storage::BitVector statesWithFixedChoice;
        if (isOrderBasedMonotonicityBackend()) {
            // Ensure choices are initialized
            if (!choices.has_value()) {
                choices.emplace(parameterLifter->getRowGroupCount(), 0u);
            }
            statesWithFixedChoice = getOrderBasedMonotonicityBackend().getChoicesToFixForPLASolver(region, *parameterLifter, dirForParameters, *choices);
        }

        // Set initial scheduler
        if (choices.has_value()) {
            solver->setInitialScheduler(std::move(choices.value()));
            if (statesWithFixedChoice.size() != 0) {
                // Choices need to be fixed after setting a scheduler
                solver->setSchedulerFixedForRowGroup(std::move(statesWithFixedChoice));
            }
        }

        if (this->currentCheckTask->isBoundSet() && solver->hasInitialScheduler()) {
            // If we reach this point, we know that after applying the hint, the x-values can only become larger (if we maximize) or smaller (if we
            // minimize).
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
        x.resize(maybeStates.getNumberOfSetBits(), storm::utility::zero<ConstantType>());
        solver->solveEquations(env, dirForParameters, x, parameterLifter->getVector());
        choices = solver->getSchedulerChoices();
        if (isValueDeltaRegionSplitEstimates()) {
            computeStateValueDeltaRegionSplitEstimates(x, choices, region.region, dirForParameters);
        }
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

template<typename SparseModelType, typename ConstantType>
void SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::computeSchedulerDeltaSplitEstimates(
    std::vector<ConstantType> const& quantitativeResult, std::vector<uint64_t> const& schedulerChoices,
    storm::storage::ParameterRegion<ParametricType> const& region, storm::solver::OptimizationDirection const& dirForParameters) {
    std::map<VariableType, ConstantType> deltaLower, deltaUpper;
    for (auto const& p : region.getVariables()) {
        deltaLower.emplace(p, storm::utility::zero<ConstantType>());
        deltaUpper.emplace(p, storm::utility::zero<ConstantType>());
    }
    auto const& choiceValuations = parameterLifter->getRowLabels();
    auto const& matrix = parameterLifter->getMatrix();
    auto const& vector = parameterLifter->getVector();

    std::vector<ConstantType> stateResults;
    for (uint64_t state = 0; state < schedulerChoices.size(); ++state) {
        uint64_t rowOffset = matrix.getRowGroupIndices()[state];
        uint64_t optimalChoice = schedulerChoices[state];
        auto const& optimalChoiceVal = choiceValuations[rowOffset + optimalChoice];
        assert(optimalChoiceVal.getUnspecifiedParameters().empty());
        stateResults.clear();
        for (uint64_t row = rowOffset; row < matrix.getRowGroupIndices()[state + 1]; ++row) {
            stateResults.push_back(matrix.multiplyRowWithVector(row, quantitativeResult) + vector[row]);
        }
        // Do this twice, once for upperbound once for lowerbound
        bool checkUpperParameters = false;
        do {
            auto const& consideredParameters = checkUpperParameters ? optimalChoiceVal.getUpperParameters() : optimalChoiceVal.getLowerParameters();
            for (auto const& p : consideredParameters) {
                // Find the 'best' choice that assigns the parameter to the other bound
                ConstantType bestValue = 0;
                bool foundBestValue = false;
                for (uint64_t choice = 0; choice < stateResults.size(); ++choice) {
                    if (choice != optimalChoice) {
                        auto const& otherBoundParsOfChoice = checkUpperParameters ? choiceValuations[rowOffset + choice].getLowerParameters()
                                                                                  : choiceValuations[rowOffset + choice].getUpperParameters();
                        if (otherBoundParsOfChoice.find(p) != otherBoundParsOfChoice.end()) {
                            ConstantType const& choiceValue = stateResults[choice];
                            if (!foundBestValue || (storm::solver::minimize(dirForParameters) ? choiceValue < bestValue : choiceValue > bestValue)) {
                                foundBestValue = true;
                                bestValue = choiceValue;
                            }
                        }
                    }
                }
                auto optimal = storm::utility::convertNumber<double>(stateResults[optimalChoice]);
                auto diff = optimal - storm::utility::convertNumber<double>(bestValue);
                if (foundBestValue) {
                    if (checkUpperParameters) {
                        deltaLower[p] += std::abs(diff);
                    } else {
                        deltaUpper[p] += std::abs(diff);
                    }
                }
            }
            checkUpperParameters = !checkUpperParameters;
        } while (checkUpperParameters);
    }

    cachedRegionSplitEstimates.clear();
    for (auto const& p : region.getVariables()) {
        // TODO: previously, the reginSplitEstimates were only used in splitting, if at least one parameter is possibly monotone. Why?

        if (auto minDelta = std::min(deltaLower[p], deltaUpper[p]); minDelta >= storm::utility::convertNumber<ConstantType>(1e-4)) {
            cachedRegionSplitEstimates.emplace(p, minDelta);
        }
    }
    // large regionsplitestimate implies that parameter p occurs as p and 1-p at least once
}

template<typename SparseModelType, typename ConstantType>
void SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::reset() {
    maybeStates.resize(0);
    resultsForNonMaybeStates.clear();
    stepBound = std::nullopt;
    instantiationChecker = nullptr;
    parameterLifter = nullptr;
    minSchedChoices = std::nullopt;
    maxSchedChoices = std::nullopt;
    x.clear();
    lowerResultBound = std::nullopt;
    upperResultBound = std::nullopt;
    // TODO: is this complete?
}

template<typename ConstantType>
std::optional<storm::storage::Scheduler<ConstantType>> getSchedulerHelper(std::optional<std::vector<uint64_t>> const& choices) {
    std::optional<storm::storage::Scheduler<ConstantType>> result;
    if (choices) {
        result.emplace(choices->size());
        uint64_t state = 0;
        for (auto const& choice : choices.value()) {
            result->setChoice(choice, state);
            ++state;
        }
    }
    return result;
}

template<typename SparseModelType, typename ConstantType>
std::optional<storm::storage::Scheduler<ConstantType>> SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::getCurrentMinScheduler() {
    return getSchedulerHelper<ConstantType>(minSchedChoices);
}

template<typename SparseModelType, typename ConstantType>
std::optional<storm::storage::Scheduler<ConstantType>> SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::getCurrentMaxScheduler() {
    return getSchedulerHelper<ConstantType>(maxSchedChoices);
}

template<typename SparseModelType, typename ConstantType>
std::shared_ptr<storm::analysis::Order> SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::extendOrder(
    std::shared_ptr<storm::analysis::Order> order, storm::storage::ParameterRegion<ParametricType> region) {
    if (this->orderExtender) {
        auto res = this->orderExtender->extendOrder(order, region);
        order = std::get<0>(res);
        if (std::get<1>(res) != order->getNumberOfStates()) {
            this->orderExtender.get().setUnknownStates(order, std::get<1>(res), std::get<2>(res));
        }
    } else {
        STORM_LOG_WARN("Extending order for RegionModelChecker not implemented");
    }
    return order;
}

template<typename SparseModelType, typename ConstantType>
void SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::extendLocalMonotonicityResult(
    storm::storage::ParameterRegion<ParametricType> const& region, std::shared_ptr<storm::analysis::Order> order,
    std::shared_ptr<storm::analysis::LocalMonotonicityResult<VariableType>> localMonotonicityResult) {
    if (this->monotoneIncrParameters && !localMonotonicityResult->isFixedParametersSet()) {
        for (auto& var : this->monotoneIncrParameters.get()) {
            localMonotonicityResult->setMonotoneIncreasing(var);
        }
        for (auto& var : this->monotoneDecrParameters.get()) {
            localMonotonicityResult->setMonotoneDecreasing(var);
        }
    }
    auto state = order->getNextDoneState(-1);
    auto const variablesAtState = parameterLifter->getOccurringVariablesAtState();
    while (state != order->getNumberOfStates()) {
        if (localMonotonicityResult->getMonotonicity(state) == nullptr) {
            auto variables = variablesAtState[state];
            if (variables.size() == 0 || order->isBottomState(state) || order->isTopState(state)) {
                localMonotonicityResult->setConstant(state);
            } else {
                for (auto const& var : variables) {
                    auto monotonicity = localMonotonicityResult->getMonotonicity(state, var);
                    if (monotonicity == Monotonicity::Unknown || monotonicity == Monotonicity::Not) {
                        monotonicity = monotonicityChecker->checkLocalMonotonicity(order, state, var, region);
                        if (monotonicity == Monotonicity::Unknown || monotonicity == Monotonicity::Not) {
                            // TODO: Skip for now?
                        } else {
                            localMonotonicityResult->setMonotonicity(state, var, monotonicity);
                        }
                    }
                }
            }
        } else {
            // Do nothing, we already checked this one
        }
        state = order->getNextDoneState(state);
    }
    auto const statesAtVariable = parameterLifter->getOccuringStatesAtVariable();
    bool allDone = true;
    for (auto const& entry : statesAtVariable) {
        auto states = entry.second;
        auto var = entry.first;
        bool done = true;
        for (auto const& state : states) {
            done &= order->contains(state) && localMonotonicityResult->getMonotonicity(state, var) != Monotonicity::Unknown;
            if (!done) {
                break;
            }
        }

        allDone &= done;
        if (done) {
            localMonotonicityResult->getGlobalMonotonicityResult()->setDoneForVar(var);
        }
    }
    if (allDone) {
        localMonotonicityResult->setDone();
        while (order->existsNextState()) {
            // Simply add the states we couldn't add sofar between =) and =( as we could find local monotonicity for all parametric states
            order->add(order->getNextStateNumber().second);
        }
        assert(order->getDoneBuilding());
    }
}

template<typename SparseModelType, typename ConstantType>
void SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::splitSmart(
    storm::storage::ParameterRegion<ParametricType>& region, std::vector<storm::storage::ParameterRegion<ParametricType>>& regionVector,
    storm::analysis::MonotonicityResult<VariableType>& monRes, bool splitForExtremum) const {
    assert(regionVector.size() == 0);

    std::multimap<double, VariableType> sortedOnValues;
    std::set<VariableType> consideredVariables;
    if (splitForExtremum) {
        if (isValueDeltaRegionSplitEstimates() && useRegionSplitEstimates) {
            STORM_LOG_INFO("Splitting based on region split estimates");
            for (auto& entry : regionSplitEstimates) {
                assert(!this->isUseMonotonicitySet() ||
                       (!monRes.isMonotone(entry.first) && this->possibleMonotoneParameters.find(entry.first) != this->possibleMonotoneParameters.end()));
                //                            sortedOnValues.insert({-(entry.second *
                //                            storm::utility::convertNumber<double>(region.getDifference(entry.first))*
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
        if (isValueDeltaRegionSplitEstimates() && useRegionSplitEstimates) {
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

bool supportsStateValueDeltaEstimates(storm::logic::Formula const& f) {
    if (f.isOperatorFormula()) {
        auto const& sub = f.asOperatorFormula().getSubformula();
        return sub.isUntilFormula() || sub.isEventuallyFormula();
    }
    return false;
}

bool supportsOrderBasedMonotonicity(storm::logic::Formula const& f) {
    // TODO: is this accurate?
    if (f.isProbabilityOperatorFormula()) {
        auto const& sub = f.asProbabilityOperatorFormula().getSubformula();
        return sub.isUntilFormula() || sub.isEventuallyFormula() || sub.isBoundedUntilFormula();
    }
    return false;
}

template<typename SparseModelType, typename ConstantType>
bool SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::isRegionSplitEstimateKindSupported(
    detail::RegionSplitEstimateKind kind, CheckTask<storm::logic::Formula, ParametricType> const& checkTask) const {
    return RegionModelChecker<ParametricType>::isRegionSplitEstimateKindSupported(kind, checkTask) ||
           (supportsStateValueDeltaEstimates(checkTask.getFormula()) && kind == detail::RegionSplitEstimateKind::StateValueDelta);
}

template<typename SparseModelType, typename ConstantType>
detail::RegionSplitEstimateKind SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::getDefaultRegionSplitEstimateKind(
    CheckTask<storm::logic::Formula, ParametricType> const& checkTask) const {
    return supportsStateValueDeltaEstimates(checkTask.getFormula()) ? detail::RegionSplitEstimateKind::StateValueDelta
                                                                    : RegionModelChecker<ParametricType>::getDefaultRegionSplitEstimateKind(checkTask);
}

template<typename SparseModelType, typename ConstantType>
std::vector<typename SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::CoefficientType>
SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::obtainRegionSplitEstimates(std::set<VariableType> const& relevantParameters) const {
    std::vector<CoefficientType> result;
    for (auto const& par : relevantParameters) {
        auto est = cachedRegionSplitEstimates.find(par);
        STORM_LOG_ASSERT(est != cachedRegionSplitEstimates.end(), "Requested region split estimate for parameter " << par.name() << " but none was generated.");
        result.push_back(storm::utility::convertNumber<CoefficientType>(est->second));
    }
    return result;
}

template<typename SparseModelType, typename ConstantType>
bool SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::isMonotonicitySupported(
    MonotonicityBackend<ParametricType> const& backend, CheckTask<storm::logic::Formula, ParametricType> const& checkTask) const {
    if (RegionModelChecker<ParametricType>::isMonotonicitySupported(backend, checkTask)) {
        return true;
    }
    return dynamic_cast<OrderBasedMonotonicityBackend<ParametricType, ConstantType> const*>(&backend) != nullptr;
}

template<typename SparseModelType, typename ConstantType>
bool SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::isOrderBasedMonotonicityBackend() const {
    return dynamic_cast<OrderBasedMonotonicityBackend<ParametricType, ConstantType>*>(this->monotonicityBackend.get()) != nullptr;
}

template<typename SparseModelType, typename ConstantType>
OrderBasedMonotonicityBackend<typename SparseModelType::ValueType, ConstantType>&
SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::getOrderBasedMonotonicityBackend() {
    return dynamic_cast<OrderBasedMonotonicityBackend<ParametricType, ConstantType>&>(*this->monotonicityBackend);
}

template<typename SparseModelType, typename ConstantType>
bool SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>::isValueDeltaRegionSplitEstimates() const {
    return this->getSpecifiedRegionSplitEstimateKind().has_value() &&
           this->getSpecifiedRegionSplitEstimateKind().value() == detail::RegionSplitEstimateKind::StateValueDelta;
}

template class SparseDtmcParameterLiftingModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double>;
template class SparseDtmcParameterLiftingModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, storm::RationalNumber>;
}  // namespace modelchecker
}  // namespace storm
