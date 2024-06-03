#include "storm-pars/modelchecker/region/SparseDtmcParameterLiftingModelChecker.h"

#include "solver/OptimizationDirection.h"
#include "storm-pars/modelchecker/region/AnnotatedRegion.h"
#include "storm-pars/modelchecker/region/RegionSplitEstimateKind.h"
#include "storm-pars/modelchecker/region/monotonicity/OrderBasedMonotonicityBackend.h"
#include "storm-pars/transformer/SparseParametricDtmcSimplifier.h"

#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/logic/FragmentSpecification.h"
#include "storm/modelchecker/prctl/helper/BaierUpperRewardBoundsComputer.h"
#include "storm/modelchecker/prctl/helper/DsMpiUpperRewardBoundsComputer.h"
#include "storm/modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/solver/MinMaxLinearEquationSolver.h"
#include "storm/solver/multiplier/Multiplier.h"
#include "storm/utility/graph.h"
#include "storm/utility/vector.h"

#include "storm/exceptions/InvalidPropertyException.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/UncheckedRequirementException.h"
#include "storm/exceptions/UnexpectedException.h"
#include "utility/constants.h"
#include "utility/logging.h"
#include "utility/macros.h"

namespace storm {
namespace modelchecker {

template<typename SparseModelType, typename ConstantType, bool Robust>
SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType, Robust>::SparseDtmcParameterLiftingModelChecker()
    : SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType, Robust>(
          std::make_unique<GeneralSolverFactoryType<ConstantType, Robust>>()) {
    // Intentionally left empty
}

template<typename SparseModelType, typename ConstantType, bool Robust>
SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType, Robust>::SparseDtmcParameterLiftingModelChecker(
    std::unique_ptr<SolverFactoryType<ConstantType, Robust>>&& solverFactory)
    : solverFactory(std::move(solverFactory)), solvingRequiresUpperRewardBounds(false) {
    // Intentionally left empty
}

template<typename SparseModelType, typename ConstantType, bool Robust>
bool SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType, Robust>::canHandle(std::shared_ptr<storm::models::ModelBase> parametricModel,
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

template<typename SparseModelType, typename ConstantType, bool Robust>
void SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType, Robust>::specify(Environment const& env,
                                                                                    std::shared_ptr<storm::models::ModelBase> parametricModel,
                                                                                    CheckTask<storm::logic::Formula, ParametricType> const& checkTask,
                                                                                    std::optional<RegionSplitEstimateKind> generateRegionSplitEstimates,
                                                                                    std::shared_ptr<MonotonicityBackend<ParametricType>> monotonicityBackend,
                                                                                    bool allowModelSimplifications) {
    STORM_LOG_ASSERT(this->canHandle(parametricModel, checkTask), "specified model and formula can not be handled by this.");
    this->specifySplitEstimates(generateRegionSplitEstimates, checkTask);
    this->specifyMonotonicity(monotonicityBackend, checkTask);
    auto dtmc = parametricModel->template as<SparseModelType>();
    if (isOrderBasedMonotonicityBackend()) {
        STORM_LOG_WARN_COND(!(allowModelSimplifications),
                            "Allowing model simplification when using order-based monotonicity is not useful, as for order-based monotonicity checking model "
                            "simplification is done as preprocessing");  // TODO: Find out where this preprocessing for monotonicity is done
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
    if constexpr (!Robust) {
        if (isOrderBasedMonotonicityBackend()) {
            getOrderBasedMonotonicityBackend().registerParameterLifterReference(*parameterLifter);
            getOrderBasedMonotonicityBackend().registerPLABoundFunction(
                [this](storm::Environment const& env, AnnotatedRegion<ParametricType>& region, storm::OptimizationDirection dir) {
                    return this->computeQuantitativeValues(env, region, dir);  // sets known value bounds within the region
                });
        }
    }
}

template<typename SparseModelType, typename ConstantType, bool Robust>
void SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType, Robust>::specifyBoundedUntilFormula(
    const CheckTask<storm::logic::BoundedUntilFormula, ConstantType>& checkTask) {
    STORM_LOG_ERROR_COND(!Robust, "Bounded until formulas not implemented for Robust PLA");
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
        parameterLifter = std::make_unique<ParameterLifterType<ParametricType, ConstantType, Robust>>(
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

template<typename SparseModelType, typename ConstantType, bool Robust>
void SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType, Robust>::specifyUntilFormula(
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
        if constexpr (Robust) {
            // Create the vector of one-step probabilities to go to target states.
            // Robust PLA doesn't support eliminating states because it gets complicated with the polynomials you know
            std::vector<ParametricType> target(this->parametricModel->getNumberOfStates(), storm::utility::zero<ParametricType>());
            storm::utility::vector::setVectorValues(target, statesWithProbability01.second, storm::utility::one<ParametricType>());

            // With Robust PLA, we cannot drop the non-maybe states out of the matrix for technical reasons
            auto rowFilter = this->parametricModel->getTransitionMatrix().getRowFilter(maybeStates);
            auto filteredMatrix = this->parametricModel->getTransitionMatrix().filterEntries(rowFilter);

            storm::storage::BitVector allTrue(maybeStates.size(), true);
            maybeStates = allTrue;

            parameterLifter = std::make_unique<ParameterLifterType<ParametricType, ConstantType, Robust>>(
                filteredMatrix, target, allTrue, allTrue, isValueDeltaRegionSplitEstimates(), isOrderBasedMonotonicityBackend());
        } else {
            // Create the vector of one-step probabilities to go to target states.
            std::vector<ParametricType> b = this->parametricModel->getTransitionMatrix().getConstrainedRowSumVector(
                storm::storage::BitVector(this->parametricModel->getTransitionMatrix().getRowCount(), true), statesWithProbability01.second);
            parameterLifter = std::make_unique<ParameterLifterType<ParametricType, ConstantType, Robust>>(
                this->parametricModel->getTransitionMatrix(), b, maybeStates, maybeStates, isValueDeltaRegionSplitEstimates(), isOrderBasedMonotonicityBackend());
        }
    }

    // We know some bounds for the results so set them
    lowerResultBound = storm::utility::zero<ConstantType>();
    upperResultBound = storm::utility::one<ConstantType>();

    // The solution of the min-max equation system will always be unique (assuming graph-preserving instantiations, every induced DTMC has the same graph
    // structure).
    auto req = solverFactory->getRequirements(env, true, true, boost::none, !Robust);
    req.clearBounds();
    STORM_LOG_THROW(!req.hasEnabledCriticalRequirement(), storm::exceptions::UncheckedRequirementException,
                    "Solver requirements " + req.getEnabledRequirementsAsString() + " not checked.");
    solverFactory->setRequirementsChecked(true);

    if (isOrderBasedMonotonicityBackend()) {
        getOrderBasedMonotonicityBackend().initializeOrderExtender(statesWithProbability01.second, statesWithProbability01.first,
                                                                   this->parametricModel->getTransitionMatrix());
    }
}

template<typename SparseModelType, typename ConstantType, bool Robust>
void SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType, Robust>::specifyReachabilityRewardFormula(
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

        if constexpr (Robust) {
            auto rowFilter = this->parametricModel->getTransitionMatrix().getRowFilter(maybeStates);
            auto filteredMatrix = this->parametricModel->getTransitionMatrix().filterEntries(rowFilter);
            storm::storage::BitVector allTrue(maybeStates.size(), true);
            maybeStates = allTrue;

            parameterLifter = std::make_unique<ParameterLifterType<ParametricType, ConstantType, Robust>>(
                filteredMatrix, b, allTrue, allTrue, isValueDeltaRegionSplitEstimates(), isOrderBasedMonotonicityBackend());
        } else {
            parameterLifter = std::make_unique<ParameterLifterType<ParametricType, ConstantType, Robust>>(
                this->parametricModel->getTransitionMatrix(), b, maybeStates, maybeStates, isValueDeltaRegionSplitEstimates(), isOrderBasedMonotonicityBackend());
        }
    }

    // We only know a lower bound for the result
    lowerResultBound = storm::utility::zero<ConstantType>();

    // The solution of the min-max equation system will always be unique (assuming graph-preserving instantiations, every induced DTMC has the same graph
    // structure).
    auto req = solverFactory->getRequirements(env, true, true, boost::none, !Robust);
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

template<typename SparseModelType, typename ConstantType, bool Robust>
void SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType, Robust>::specifyCumulativeRewardFormula(
    const CheckTask<storm::logic::CumulativeRewardFormula, ConstantType>& checkTask) {
    STORM_LOG_ERROR_COND(!Robust, "Not implemented for robust mode.");
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

    parameterLifter = std::make_unique<ParameterLifterType<ParametricType, ConstantType, Robust>>(this->parametricModel->getTransitionMatrix(), b,
                                                                                                          maybeStates, maybeStates);

    // We only know a lower bound for the result
    lowerResultBound = storm::utility::zero<ConstantType>();

    // No requirements for bounded reward formula
    solverFactory->setRequirementsChecked(true);

    STORM_LOG_WARN_COND(!isOrderBasedMonotonicityBackend(), "Order-based monotonicity not used for cumulative reward formula.");
}

template<typename SparseModelType, typename ConstantType, bool Robust>
storm::modelchecker::SparseInstantiationModelChecker<SparseModelType, ConstantType>&
SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType, Robust>::getInstantiationCheckerSAT() {
    if (!instantiationCheckerSAT) {
        instantiationCheckerSAT =
            std::make_unique<storm::modelchecker::SparseDtmcInstantiationModelChecker<SparseModelType, ConstantType>>(*this->parametricModel);
        instantiationCheckerSAT->specifyFormula(this->currentCheckTask->template convertValueType<ParametricType>());
        instantiationCheckerSAT->setInstantiationsAreGraphPreserving(true);
    }
    return *instantiationCheckerSAT;
}

template<typename SparseModelType, typename ConstantType, bool Robust>
storm::modelchecker::SparseInstantiationModelChecker<SparseModelType, ConstantType>&
SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType, Robust>::getInstantiationCheckerVIO() {
    if (!instantiationCheckerVIO) {
        instantiationCheckerVIO =
            std::make_unique<storm::modelchecker::SparseDtmcInstantiationModelChecker<SparseModelType, ConstantType>>(*this->parametricModel);
        instantiationCheckerVIO->specifyFormula(this->currentCheckTask->template convertValueType<ParametricType>());
        instantiationCheckerVIO->setInstantiationsAreGraphPreserving(true);
    }
    return *instantiationCheckerVIO;
}

template<typename SparseModelType, typename ConstantType, bool Robust>
storm::modelchecker::SparseInstantiationModelChecker<SparseModelType, ConstantType>&
SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType, Robust>::getInstantiationChecker() {
    if (!instantiationChecker) {
        instantiationChecker =
            std::make_unique<storm::modelchecker::SparseDtmcInstantiationModelChecker<SparseModelType, ConstantType>>(*this->parametricModel);
        instantiationChecker->specifyFormula(this->currentCheckTask->template convertValueType<ParametricType>());
        instantiationChecker->setInstantiationsAreGraphPreserving(true);
    }
    return *instantiationChecker;
}

template<typename SparseModelType, typename ConstantType, bool Robust>
std::vector<ConstantType> SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType, Robust>::computeQuantitativeValues(
    Environment const& env, AnnotatedRegion<ParametricType>& region, storm::solver::OptimizationDirection const& dirForParameters) {
    if (maybeStates.empty()) {
        this->updateKnownValueBoundInRegion(region, dirForParameters, resultsForNonMaybeStates);
        return resultsForNonMaybeStates;
    }
    parameterLifter->specifyRegion(region.region, dirForParameters);

    if (stepBound) {
        if constexpr (!Robust) {
            assert(*stepBound > 0);
            x = std::vector<ConstantType>(maybeStates.getNumberOfSetBits(), storm::utility::zero<ConstantType>());
            auto multiplier = storm::solver::MultiplierFactory<ConstantType>().create(env, parameterLifter->getMatrix());
            multiplier->repeatedMultiplyAndReduce(env, dirForParameters, x, &parameterLifter->getVector(), *stepBound);
        } else {
            STORM_LOG_ERROR("Cannot check step-bounded formulas in robust mode.");
        }
    } else {
        auto solver = solverFactory->create(env, parameterLifter->getMatrix());
        solver->setHasUniqueSolution();
        solver->setHasNoEndComponents();
        if (lowerResultBound)
            solver->setLowerBound(lowerResultBound.value());
        if (upperResultBound) {
            solver->setUpperBound(upperResultBound.value());
        } else if (solvingRequiresUpperRewardBounds) {
            if constexpr (!Robust) {
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
            } else {
                STORM_LOG_ERROR("Cannot use upper reward bounds in robust mode.");
            }
        }
        solver->setTrackScheduler(true);

        // Get reference to relevant scheduler choices
        auto& choices = storm::solver::minimize(dirForParameters) ? minSchedChoices : maxSchedChoices;

        // Potentially fix some choices if order based monotonicity is known
        if constexpr (!Robust) {
            storm::storage::BitVector statesWithFixedChoice;
            if (isOrderBasedMonotonicityBackend()) {
                // Ensure choices are initialized
                if (!choices.has_value()) {
                    choices.emplace(parameterLifter->getRowGroupCount(), 0u);
                }
                statesWithFixedChoice = getOrderBasedMonotonicityBackend().getChoicesToFixForPLASolver(region, dirForParameters, *choices);
            }

            // Set initial scheduler
            if (choices.has_value()) {
                solver->setInitialScheduler(std::move(choices.value()));
                if (statesWithFixedChoice.size() != 0) {
                    // Choices need to be fixed after setting a scheduler
                    solver->setSchedulerFixedForRowGroup(std::move(statesWithFixedChoice));
                }
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
            computeStateValueDeltaRegionSplitEstimates(x, *choices, region.region, dirForParameters);
        }
    }

    // Get the result for the complete model (including maybestates)
    std::vector<ConstantType> result = resultsForNonMaybeStates;
    auto maybeStateResIt = x.begin();
    for (auto const& maybeState : maybeStates) {
        result[maybeState] = *maybeStateResIt;
        ++maybeStateResIt;
    }
    this->updateKnownValueBoundInRegion(region, dirForParameters, result);
    return result;
}

template<typename SparseModelType, typename ConstantType, bool Robust>
void SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType, Robust>::computeStateValueDeltaRegionSplitEstimates(
    std::vector<ConstantType> const& quantitativeResult, std::vector<uint64_t> const& schedulerChoices,
    storm::storage::ParameterRegion<ParametricType> const& region, storm::solver::OptimizationDirection const& dirForParameters) {
    auto const& matrix = parameterLifter->getMatrix();
    auto const& vector = parameterLifter->getVector();
    switch (*this->specifiedRegionSplitEstimateKind) {
    case RegionSplitEstimateKind::StateValueDelta:
        if constexpr (Robust) {
            STORM_LOG_ERROR("State-value-delta split estimates not supported for robust PLA.");
        } else {
            std::map<VariableType, ConstantType> deltaLower, deltaUpper;
            for (auto const& p : region.getVariables()) {
                deltaLower.emplace(p, storm::utility::zero<ConstantType>());
                deltaUpper.emplace(p, storm::utility::zero<ConstantType>());
            }
            auto const& choiceValuations = parameterLifter->getRowLabels();

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
                        auto const& optimal = stateResults[optimalChoice];
                        auto diff = storm::utility::abs<ConstantType>(optimal - storm::utility::convertNumber<ConstantType>(bestValue));
                        if (foundBestValue) {
                            if (checkUpperParameters) {
                                deltaLower[p] += diff;
                            } else {
                                deltaUpper[p] += diff;
                            }
                        }
                    }
                    checkUpperParameters = !checkUpperParameters;
                } while (checkUpperParameters);

                cachedRegionSplitEstimates.clear();
                for (auto const& p : region.getVariables()) {
                    // TODO: previously, the reginSplitEstimates were only used in splitting if at least one parameter is possibly monotone. Why?
                    auto minDelta = std::min(deltaLower[p], deltaUpper[p]);
                    cachedRegionSplitEstimates.emplace(p, minDelta);

                    // Previously - why only insert some regionsplitestimates?????????????????????
                    // if (auto minDelta = std::min(deltaLower[p], deltaUpper[p]); minDelta >= storm::utility::convertNumber<ConstantType>(1e-4)) {
                    //     cachedRegionSplitEstimates.emplace(p, minDelta);
                    // }
                }
            }
            // large regionsplitestimate implies that parameter p occurs as p and 1-p at least once
        }
        break;
    case RegionSplitEstimateKind::MinMaxDelta:
    case RegionSplitEstimateKind::MinMaxDeltaWeighted:
        if constexpr (!Robust) {
            STORM_LOG_ERROR("Min-max-delta split estimates not supported for non-robust PLA. (TODO)");
        } else {
            cachedRegionSplitEstimates.clear();
            for (auto const& p : region.getVariables()) {
                cachedRegionSplitEstimates.emplace(p, utility::zero<ConstantType>());
            }
            // Assumption: Only one parameter per state
            for (uint64_t state = 0; state < vector.size(); ++state) {
                auto variables = parameterLifter->getOccurringVariablesAtState().at(state);
                if (variables.size() == 0) {
                    continue;
                }
                STORM_LOG_ERROR_COND(variables.size() == 1, "Cannot compute state-value-delta split estimates in robust mode if there are states with multiple parameters.");

                std::vector<ConstantType> results;

                for (auto const& direction : {OptimizationDirection::Maximize, OptimizationDirection::Minimize}) {
                    // Do a step of robust value iteration
                    // TODO I think it is a problem if we have probabilities and a state that is going to the vector, we don't count that
                    // Currently "fixed in preprocessing"
                    // It's different for rewards (same problem in ValueIterationOperator.h, search for word "octopus" in codebase)
                    ConstantType remainingValue = utility::one<ConstantType>();
                    ConstantType result = utility::zero<ConstantType>();

                    std::vector<std::pair<ConstantType, ConstantType>> robustOrder;

                    for (auto const& entry : matrix.getRow(state)) {
                        auto const lower = entry.getValue().lower();
                        result += quantitativeResult[entry.getColumn()] * lower;
                        remainingValue -= lower;
                        auto const diameter = entry.getValue().upper() - lower;
                        if (!storm::utility::isZero(diameter)) {
                            robustOrder.emplace_back(quantitativeResult[entry.getColumn()], diameter);
                        }
                    }

                    std::sort(robustOrder.begin(), robustOrder.end(), [direction](const std::pair<ConstantType, ConstantType>& a,
                            const std::pair<ConstantType, ConstantType>& b) {
                        if (direction == OptimizationDirection::Maximize) {
                            return a.first > b.first;
                        } else {
                            return a.first < b.first;
                        }
                    });

                    for (auto const& pair : robustOrder) {
                        auto availableMass = std::min(pair.second, remainingValue);
                        result += availableMass * pair.first;
                        remainingValue -= availableMass;
                    }

                    results.push_back(result);
                }

                cachedRegionSplitEstimates.at(*variables.begin()) += results[0] - results[1];
            }
        }
        break;
    default:
        STORM_LOG_ERROR("Region split estimate kind not handled by SparseDtmcParameterLiftingModelChecker.");
    }
}

template<typename SparseModelType, typename ConstantType, bool Robust>
void SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType, Robust>::reset() {
    maybeStates.resize(0);
    resultsForNonMaybeStates.clear();
    stepBound = std::nullopt;
    instantiationChecker = nullptr;
    instantiationCheckerSAT = nullptr;
    instantiationCheckerVIO = nullptr;
    parameterLifter = nullptr;
    minSchedChoices = std::nullopt;
    maxSchedChoices = std::nullopt;
    x.clear();
    lowerResultBound = std::nullopt;
    upperResultBound = std::nullopt;
    cachedRegionSplitEstimates.clear();
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

template<typename SparseModelType, typename ConstantType, bool Robust>
std::optional<storm::storage::Scheduler<ConstantType>> SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType, Robust>::getCurrentMinScheduler() {
    return getSchedulerHelper<ConstantType>(minSchedChoices);
}

template<typename SparseModelType, typename ConstantType, bool Robust>
std::optional<storm::storage::Scheduler<ConstantType>> SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType, Robust>::getCurrentMaxScheduler() {
    return getSchedulerHelper<ConstantType>(maxSchedChoices);
}

bool supportsStateValueDeltaEstimates(storm::logic::Formula const& f) {
    if (f.isOperatorFormula()) {
        auto const& sub = f.asOperatorFormula().getSubformula();
        return sub.isUntilFormula() || sub.isEventuallyFormula();
    }
    return false;
}

bool supportsOrderBasedMonotonicity(storm::logic::Formula const& f) {
    if (f.isProbabilityOperatorFormula()) {
        auto const& sub = f.asProbabilityOperatorFormula().getSubformula();
        return sub.isUntilFormula() || sub.isEventuallyFormula() || sub.isBoundedUntilFormula();
    }
    return false;
}

template<typename SparseModelType, typename ConstantType, bool Robust>
bool SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType, Robust>::isRegionSplitEstimateKindSupported(
    RegionSplitEstimateKind kind, CheckTask<storm::logic::Formula, ParametricType> const& checkTask) const {
    return RegionModelChecker<ParametricType>::isRegionSplitEstimateKindSupported(kind, checkTask) ||
           (!Robust && supportsStateValueDeltaEstimates(checkTask.getFormula()) && kind == RegionSplitEstimateKind::StateValueDelta) ||
           (Robust && (kind == RegionSplitEstimateKind::MinMaxDelta || kind == RegionSplitEstimateKind::MinMaxDeltaWeighted));
}

template<typename SparseModelType, typename ConstantType, bool Robust>
RegionSplitEstimateKind SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType, Robust>::getDefaultRegionSplitEstimateKind(
    CheckTask<storm::logic::Formula, ParametricType> const& checkTask) const {
    return !Robust && supportsStateValueDeltaEstimates(checkTask.getFormula()) ? RegionSplitEstimateKind::StateValueDelta
                                                                    : RegionModelChecker<ParametricType>::getDefaultRegionSplitEstimateKind(checkTask);
}

template<typename SparseModelType, typename ConstantType, bool Robust>
std::vector<typename SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType, Robust>::CoefficientType>
SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType, Robust>::obtainRegionSplitEstimates(std::set<VariableType> const& relevantParameters) const {
    if (isValueDeltaRegionSplitEstimates()) {
        // Cached region split estimates are value-delta
        std::vector<CoefficientType> result;
        for (auto const& par : relevantParameters) {
            auto est = cachedRegionSplitEstimates.find(par);
            STORM_LOG_ASSERT(est != cachedRegionSplitEstimates.end(), "Requested region split estimate for parameter " << par.name() << " but none was generated.");
            result.push_back(storm::utility::convertNumber<CoefficientType>(est->second));
        }
        return result;
    } else {
        // Call super method, which might support the estimate type
        return RegionModelChecker<ParametricType>::obtainRegionSplitEstimates(relevantParameters);
    }
}

template<typename SparseModelType, typename ConstantType, bool Robust>
bool SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType, Robust>::isMonotonicitySupported(
    MonotonicityBackend<ParametricType> const& backend, CheckTask<storm::logic::Formula, ParametricType> const& checkTask) const {
    if (backend.requiresInteractionWithRegionModelChecker()) {
        return dynamic_cast<OrderBasedMonotonicityBackend<ParametricType, ConstantType> const*>(&backend) != nullptr &&
               supportsOrderBasedMonotonicity(checkTask.getFormula());
    } else {
        return true;
    }
}

template<typename SparseModelType, typename ConstantType, bool Robust>
bool SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType, Robust>::isOrderBasedMonotonicityBackend() const {
    return dynamic_cast<OrderBasedMonotonicityBackend<ParametricType, ConstantType>*>(this->monotonicityBackend.get()) != nullptr;
}

template<typename SparseModelType, typename ConstantType, bool Robust>
OrderBasedMonotonicityBackend<typename SparseModelType::ValueType, ConstantType>&
SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType, Robust>::getOrderBasedMonotonicityBackend() {
    return dynamic_cast<OrderBasedMonotonicityBackend<ParametricType, ConstantType>&>(*this->monotonicityBackend);
}

template<typename SparseModelType, typename ConstantType, bool Robust>
bool SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType, Robust>::isValueDeltaRegionSplitEstimates() const {
    return this->getSpecifiedRegionSplitEstimateKind().has_value() &&
           (this->getSpecifiedRegionSplitEstimateKind().value() == RegionSplitEstimateKind::StateValueDelta ||
            this->getSpecifiedRegionSplitEstimateKind().value() == RegionSplitEstimateKind::MinMaxDelta ||
            this->getSpecifiedRegionSplitEstimateKind().value() == RegionSplitEstimateKind::MinMaxDeltaWeighted);
}

template class SparseDtmcParameterLiftingModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double, false>;
template class SparseDtmcParameterLiftingModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double, true>;
template class SparseDtmcParameterLiftingModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, storm::RationalNumber, false>;
}  // namespace modelchecker
}  // namespace storm
