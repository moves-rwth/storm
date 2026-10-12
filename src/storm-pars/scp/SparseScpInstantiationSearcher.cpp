#include "storm-pars/scp/SparseScpInstantiationSearcher.h"

#include <algorithm>

#include "storm-pars/modelchecker/instantiation/SparseDtmcInstantiationModelChecker.h"
#include "storm-pars/modelchecker/instantiation/SparseMdpInstantiationModelChecker.h"
#include "storm-pars/storage/ParameterRegion.h"
#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/logic/Formulas.h"
#include "storm/modelchecker/CheckTask.h"
#include "storm/modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/utility/SignalHandler.h"
#include "storm/utility/constants.h"
#include "storm/utility/graph.h"
#include "storm/utility/macros.h"

namespace storm::pars::scp {

template<typename SparseModelType, typename ConstantType>
SparseScpInstantiationSearcher<SparseModelType, ConstantType>::SparseScpInstantiationSearcher(SparseModelType const& model, ScpOptions options)
    : model(model), options(options) {}

template<typename SparseModelType, typename ConstantType>
ScpResult<ConstantType> SparseScpInstantiationSearcher<SparseModelType, ConstantType>::run(
    storm::Environment const& env, std::shared_ptr<storm::pars::FeasibilitySynthesisTask const> const& task) {
    // Validated here (not just by ScpSettings' CLI argument validators) since ScpOptions can also
    // be constructed directly by a library caller, bypassing the CLI entirely.
    STORM_LOG_THROW(options.trustRegionInitial > 1.0, storm::exceptions::NotSupportedException, "ScpOptions::trustRegionInitial must be > 1.");
    STORM_LOG_THROW(options.trustRegionFactor > 1.0, storm::exceptions::NotSupportedException, "ScpOptions::trustRegionFactor must be > 1.");
    STORM_LOG_THROW(options.trustRegionMax > 1.0, storm::exceptions::NotSupportedException, "ScpOptions::trustRegionMax must be > 1.");
    STORM_LOG_THROW(options.trustRegionMinExcess >= 0.0, storm::exceptions::NotSupportedException, "ScpOptions::trustRegionMinExcess must be >= 0.");
    STORM_LOG_THROW(options.maxIterations >= 1, storm::exceptions::NotSupportedException, "ScpOptions::maxIterations must be >= 1.");

    STORM_LOG_THROW(task->getFormula().isProbabilityOperatorFormula() || task->getFormula().isRewardOperatorFormula(),
                    storm::exceptions::NotSupportedException, "SCP currently only supports probability- and reward operator formulas.");
    bool const isReward = task->getFormula().isRewardOperatorFormula();
    STORM_LOG_THROW(task->getFormula().asOperatorFormula().getSubformula().isEventuallyFormula(), storm::exceptions::NotSupportedException,
                    "SCP currently only supports unbounded reachability (F) properties.");
    STORM_LOG_THROW(task->isBoundSet(), storm::exceptions::NotSupportedException, "SCP requires an explicitly given bound.");

    storm::logic::Bound const& bound = task->getBound();
    ThresholdDirection direction = bound.isLowerBound() ? ThresholdDirection::AtLeast : ThresholdDirection::AtMost;
    double threshold = bound.threshold.evaluateAsDouble();

    // The robust reading of the property (see ThresholdDirection's doc comment): AtLeast pairs
    // with Pmin (even the worst-case scheduler clears the threshold), AtMost with Pmax. This is
    // the same default storm's own CheckTask::updateOperatorInformation uses for an unqualified
    // bounded formula on a nondeterministic model.
    storm::solver::OptimizationDirection schedulerDirection =
        (direction == ThresholdDirection::AtLeast) ? storm::solver::OptimizationDirection::Minimize : storm::solver::OptimizationDirection::Maximize;

    // Target states, via a plain propositional check of the (label-based) subformula of "F ...".
    auto const& targetSubformula = task->getFormula().asOperatorFormula().getSubformula().asEventuallyFormula().getSubformula();
    storm::modelchecker::SparsePropositionalModelChecker<SparseModelType> propositionalChecker(model);
    storm::modelchecker::CheckTask<storm::logic::Formula, ParametricType> targetCheckTask(targetSubformula);
    storm::storage::BitVector targetStates = propositionalChecker.check(targetCheckTask)
                                                 ->template asExplicitQualitativeCheckResult<ParametricType>()
                                                 .getTruthValuesVector();
    storm::storage::BitVector allStates(model.getNumberOfStates(), true);

    // For a probability property: zeroStates/oneStates are Prob0/Prob1. For a reward property:
    // zeroStates are the target states themselves (Rew0 -- no more reward accrues once already at
    // the target), oneStates has no meaning and stays empty, and infiniteStates are the states
    // with infinite expected reward (RewInf). Both classifications must match schedulerDirection
    // (see ScpLpEncoder's class-level doc comment for why the qualitative sets have to agree with
    // the LP's constraint/objective pairing).
    storm::storage::BitVector zeroStates, oneStates, infiniteStates;
    storm::storage::BitVector excludedRows(model.getTransitionMatrix().getRowCount(), false);
    if constexpr (std::is_same_v<SparseModelType, storm::models::sparse::Mdp<ParametricType>>) {
        if (isReward) {
            zeroStates = targetStates;
            oneStates = storm::storage::BitVector(model.getNumberOfStates(), false);
            infiniteStates = storm::solver::minimize(schedulerDirection)
                                 ? storm::utility::graph::performProb1E(model.getTransitionMatrix(), model.getTransitionMatrix().getRowGroupIndices(),
                                                                        model.getBackwardTransitions(), allStates, targetStates)
                                 : storm::utility::graph::performProb1A(model.getTransitionMatrix(), model.getTransitionMatrix().getRowGroupIndices(),
                                                                        model.getBackwardTransitions(), allStates, targetStates);
            infiniteStates.complement();
            // A "maybe" MDP state can have both actions that stay finite and actions that lead
            // straight into an infinite-value state; only the former can ever be part of an
            // optimal scheduler for a state that is itself finite, so those rows are excluded
            // individually rather than by excluding the whole state.
            storm::storage::BitVector maybeStates = ~(zeroStates | infiniteStates);
            excludedRows = ~model.getTransitionMatrix().getRowFilter(maybeStates, ~infiniteStates);
        } else {
            auto probs = storm::solver::maximize(schedulerDirection) ? storm::utility::graph::performProb01Max(model, allStates, targetStates)
                                                                      : storm::utility::graph::performProb01Min(model, allStates, targetStates);
            zeroStates = std::move(probs.first);
            oneStates = std::move(probs.second);
            infiniteStates = storm::storage::BitVector(model.getNumberOfStates(), false);
        }
    } else {
        static_assert(std::is_same_v<SparseModelType, storm::models::sparse::Dtmc<ParametricType>>,
                      "SparseScpInstantiationSearcher only supports Mdp and Dtmc models.");
        if (isReward) {
            zeroStates = targetStates;
            oneStates = storm::storage::BitVector(model.getNumberOfStates(), false);
            infiniteStates = storm::utility::graph::performProb1(model.getBackwardTransitions(), allStates, targetStates);
            infiniteStates.complement();
            // A DTMC state has only one action; if it led into an infinite-value successor it
            // would transitively be infinite itself already (i.e. already excluded above), so no
            // row-level exclusion beyond the state level is ever needed here.
        } else {
            auto probs = storm::utility::graph::performProb01(model, allStates, targetStates);
            zeroStates = std::move(probs.first);
            oneStates = std::move(probs.second);
            infiniteStates = storm::storage::BitVector(model.getNumberOfStates(), false);
        }
    }

    storm::models::sparse::StandardRewardModel<ParametricType> const* rewardModel = nullptr;
    if (isReward) {
        auto const& rewardFormula = task->getFormula().asRewardOperatorFormula();
        if (rewardFormula.hasRewardModelName()) {
            STORM_LOG_THROW(model.hasRewardModel(rewardFormula.getRewardModelName()), storm::exceptions::NotSupportedException,
                            "The model has no reward model named '" << rewardFormula.getRewardModelName() << "'.");
            rewardModel = &model.getRewardModel(rewardFormula.getRewardModelName());
        } else {
            STORM_LOG_THROW(model.hasUniqueRewardModel(), storm::exceptions::NotSupportedException,
                            "The reward formula does not name a reward model, and the model does not have a unique reward model either.");
            rewardModel = &model.getUniqueRewardModel();
        }
    }

    AffinePMdpDecomposition decomposition(model, rewardModel);

    // Always work with a real region object -- either the caller's, or a synthesized [0,1]^n --
    // so region coverage and graph-preservation get checked uniformly in both cases. This matters:
    // the *default* [0,1]^n box is, if anything, the case most likely to actually violate
    // graph-preservation (e.g. a plain "1-p" transition hits 0 at p=1, which is inside the default
    // box), so skipping the check just because no explicit region was given would be backwards.
    storm::storage::ParameterRegion<ParametricType> region;
    if (task->isRegionSet()) {
        region = task->getRegion();
        for (auto const& var : decomposition.getParameters()) {
            STORM_LOG_THROW(region.getVariables().count(var) > 0, storm::exceptions::NotSupportedException,
                            "The given region does not cover parameter '" << var << "', which occurs in the model.");
        }
    } else {
        Valuation lower, upper;
        for (auto const& var : decomposition.getParameters()) {
            lower[var] = storm::utility::zero<CoefficientType>();
            upper[var] = storm::utility::one<CoefficientType>();
        }
        region = storm::storage::ParameterRegion<ParametricType>(std::move(lower), std::move(upper));
    }

    std::optional<bool> graphPreserving = decomposition.isGraphPreserving(region);
    if (graphPreserving.has_value() && !*graphPreserving) {
        // Not a hard error: soundness never depends on this (a Feasible result is always gated by
        // an exact model check of the specific candidate, never by the LP's Prob0/Prob1-informed
        // linearization), and this situation is extremely common and often harmless -- e.g. the
        // default [0,1]^n region always technically "touches" the boundary for a plain "1-p"-style
        // transition. What it *can* cost is search effectiveness: the Prob0/Prob1 classification
        // used to build the LP is precomputed once from the model's graph structure and may be
        // wrong for the (measure-zero, in practice rarely-hit-exactly) subset of the region where a
        // transition actually reaches 0 or 1, making the linearization a poor local model there.
        STORM_LOG_WARN(
            "The region is not graph-preserving: some transition probability reaches 0 or 1 somewhere inside it (other than as a "
            "structural constant, e.g. a self-loop). This does not affect soundness (every accepted witness is still exactly "
            "model-checked), but the search may be less effective near the affected parameter values.");
    } else if (!graphPreserving.has_value()) {
        STORM_LOG_WARN("The region has too many parameters ("
                       << decomposition.getParameters().size()
                       << ") to verify graph-preservation exhaustively (would require enumerating 2^n vertices); proceeding without checking it. "
                          "As with an explicit violation, this cannot affect soundness, only search effectiveness.");
    }

    std::map<VariableType, std::pair<double, double>> parameterBounds;
    for (auto const& var : decomposition.getParameters()) {
        parameterBounds[var] = {storm::utility::convertNumber<double>(region.getLowerBoundary(var)),
                                storm::utility::convertNumber<double>(region.getUpperBoundary(var))};
    }
    Valuation initialValuation = region.getCenterPoint();

    STORM_LOG_THROW(model.getInitialStates().getNumberOfSetBits() == 1, storm::exceptions::NotSupportedException,
                    "SCP requires a model with a unique initial state.");
    uint64_t initialState = model.getInitialStates().getNextSetIndex(0);

    if (zeroStates.get(initialState) || oneStates.get(initialState) || infiniteStates.get(initialState)) {
        // The initial state's value is fixed (0, 1, or infinity) regardless of the parameter
        // valuation, given the region's assumed graph-preservation -- known directly from the
        // graph analysis above, no LP or exact model check needed (and none of the SCP-specific
        // machinery below is set up to handle this degenerate case: ScpLpEncoder asserts its
        // initial state is never a zero/one/infinite state).
        ConstantType constantValue = oneStates.get(initialState)      ? storm::utility::one<ConstantType>()
                                     : infiniteStates.get(initialState) ? storm::utility::infinity<ConstantType>()
                                                                        : storm::utility::zero<ConstantType>();
        return ScpResult<ConstantType>{bound.isSatisfied(constantValue) ? ScpStatus::Feasible : ScpStatus::Infeasible, initialValuation, constantValue, 0};
    }

    std::vector<uint64_t> rowGroupIndices(model.getTransitionMatrix().getRowGroupIndices().begin(),
                                          model.getTransitionMatrix().getRowGroupIndices().end());
    PropertyKind propertyKind = isReward ? PropertyKind::Reward : PropertyKind::Probability;
    ScpLpEncoder encoder(decomposition, rowGroupIndices, zeroStates, oneStates, infiniteStates, excludedRows, initialState, parameterBounds, threshold,
                        direction, propertyKind, options);

    storm::modelchecker::CheckTask<storm::logic::Formula, ParametricType> checkTask(task->getFormula(), /*onlyInitialStatesRelevant=*/false);

    if constexpr (std::is_same_v<SparseModelType, storm::models::sparse::Mdp<ParametricType>>) {
        if (checkTask.isOptimizationDirectionSet()) {
            // The formula explicitly said Pmax=? or Pmin=?: it must agree with what the threshold
            // direction implies (see ThresholdDirection's doc comment); the LP's own linearization
            // is built for schedulerDirection specifically, and would be linearizing the wrong
            // extremal otherwise -- not unsound (the exact check still gates every accept), but
            // pointless, so V1 rejects the mismatch rather than silently ignoring the formula's
            // explicit direction or the threshold's.
            STORM_LOG_THROW(checkTask.getOptimizationDirection() == schedulerDirection, storm::exceptions::NotSupportedException,
                            "SCP requires the formula's optimization direction (if explicitly given, e.g. Pmax=?/Pmin=?) to match the "
                            "robust reading implied by the threshold direction (AtLeast -> Pmin, AtMost -> Pmax); mismatched combinations "
                            "like 'Pmax >= lambda' are not supported.");
        } else {
            checkTask.setOptimizationDirection(schedulerDirection);
        }
        storm::modelchecker::SparseMdpInstantiationModelChecker<SparseModelType, ConstantType> instantiationChecker(env, model, /*produceScheduler=*/false);
        instantiationChecker.setInstantiationsAreGraphPreserving(true);
        instantiationChecker.specifyFormula(checkTask);
        return runLoop(env, instantiationChecker, encoder, bound, direction, initialState, initialValuation);
    } else {
        storm::modelchecker::SparseDtmcInstantiationModelChecker<SparseModelType, ConstantType> instantiationChecker(env, model);
        instantiationChecker.setInstantiationsAreGraphPreserving(true);
        instantiationChecker.specifyFormula(checkTask);
        return runLoop(env, instantiationChecker, encoder, bound, direction, initialState, initialValuation);
    }
}

template<typename SparseModelType, typename ConstantType>
template<typename InstantiationChecker>
ScpResult<ConstantType> SparseScpInstantiationSearcher<SparseModelType, ConstantType>::runLoop(
    storm::Environment const& env, InstantiationChecker& instantiationChecker, ScpLpEncoder const& encoder, storm::logic::Bound const& bound,
    ThresholdDirection direction, uint64_t initialState, Valuation const& initialValuation) const {
    // Returns the model-checked value vector at ConstantType precision, never downcast to double --
    // this is what makes the accept/reject and threshold-satisfaction decisions below exact when
    // ConstantType is (e.g. storm::RationalNumber), not just "double, but computed by an exact
    // checker and then rounded away". double is only introduced where it's unavoidable: the LP
    // solver itself (see toDoubleVec/toDouble below) is inherently double-precision.
    auto exactCheck = [&](Valuation const& valuation) -> std::vector<ConstantType> {
        auto result = instantiationChecker.check(env, valuation);
        return result->template asExplicitQuantitativeCheckResult<ConstantType>().getFiniteValueVector();
    };
    auto toExact = [](std::map<VariableType, double> const& v) -> Valuation {
        Valuation exact;
        for (auto const& [var, val] : v) {
            exact[var] = storm::utility::convertNumber<CoefficientType>(val);
        }
        return exact;
    };
    auto toDouble = [](Valuation const& v) -> std::map<VariableType, double> {
        std::map<VariableType, double> result;
        for (auto const& [var, val] : v) {
            result[var] = storm::utility::convertNumber<double>(val);
        }
        return result;
    };
    auto toDoubleVec = [](std::vector<ConstantType> const& v) -> std::vector<double> {
        std::vector<double> result(v.size());
        for (uint64_t i = 0; i < v.size(); ++i) {
            result[i] = storm::utility::convertNumber<double>(v[i]);
        }
        return result;
    };

    Valuation vHat = initialValuation;
    std::vector<ConstantType> pHat = exactCheck(vHat);
    ConstantType betaStar = pHat.at(initialState);

    ScpResult<ConstantType> best{ScpStatus::Infeasible, vHat, betaStar, 0};
    if (bound.isSatisfied(betaStar)) {
        best.status = ScpStatus::Feasible;
        return best;
    }

    double delta = options.trustRegionInitial;

    uint64_t iteration = 0;
    for (; iteration < options.maxIterations; ++iteration) {
        // Polls storm's global resource-limit mechanism (--timeout / Ctrl-C / other termination
        // signals; storm::utility::resources::installSignalHandler is set up once for the whole
        // process) rather than tracking a second, SCP-local wall clock.
        if (storm::utility::resources::isTerminate()) {
            // best.iterations is deliberately left as-is here (the iteration at which `best` was
            // last established), not overwritten with the current iteration count -- see
            // ScpResult's doc comment for why those are different things.
            best.status = ScpStatus::Timeout;
            return best;
        }
        if (delta - 1.0 <= options.trustRegionMinExcess) {
            best.status = ScpStatus::Infeasible;
            return best;
        }

        EncodedLp lp = encoder.encode(env, toDouble(vHat), toDoubleVec(pHat), delta);
        lp.solver->optimize();
        if (!lp.solver->isOptimal()) {
            delta = (delta - 1.0) / options.trustRegionFactor + 1.0;
            continue;
        }

        std::map<VariableType, double> candidateDouble;
        for (auto const& [var, lpVar] : lp.parameterVariables) {
            candidateDouble[var] = lp.solver->getContinuousValue(lpVar);
        }
        Valuation candidate = toExact(candidateDouble);

        std::vector<ConstantType> valueVector = exactCheck(candidate);
        ConstantType beta = valueVector.at(initialState);

        if (bound.isSatisfied(beta)) {
            return ScpResult<ConstantType>{ScpStatus::Feasible, candidate, beta, iteration + 1};
        }

        bool improved = (direction == ThresholdDirection::AtLeast) ? (beta > betaStar) : (beta < betaStar);
        if (improved) {
            vHat = candidate;
            pHat = std::move(valueVector);
            betaStar = beta;
            best = ScpResult<ConstantType>{ScpStatus::Infeasible, vHat, betaStar, iteration + 1};
            delta = std::min(options.trustRegionMax, (delta - 1.0) * options.trustRegionFactor + 1.0);
        } else {
            delta = (delta - 1.0) / options.trustRegionFactor + 1.0;
        }
    }

    best.status = ScpStatus::MaxIterationsReached;
    return best;
}

template class SparseScpInstantiationSearcher<storm::models::sparse::Mdp<storm::RationalFunction>, double>;
template class SparseScpInstantiationSearcher<storm::models::sparse::Dtmc<storm::RationalFunction>, double>;
// ConstantType = storm::RationalNumber: the model-checking soundness gate, the accept/reject and
// threshold-satisfaction decisions, and the returned ScpResult::value all run/are reported at exact
// rational precision (see ScpResult's doc comment). The LP relaxation itself is still double -- its
// candidate is only ever a proposal, never trusted directly.
template class SparseScpInstantiationSearcher<storm::models::sparse::Mdp<storm::RationalFunction>, storm::RationalNumber>;
template class SparseScpInstantiationSearcher<storm::models::sparse::Dtmc<storm::RationalFunction>, storm::RationalNumber>;

}  // namespace storm::pars::scp
