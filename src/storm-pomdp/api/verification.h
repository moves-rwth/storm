#pragma once

#include "storm-pomdp/beliefs/verification/BeliefBasedModelCheckerOptions.h"
#include "storm-pomdp/beliefs/verification/BeliefBasedModelCheckerResult.h"
#include "storm/environment/Environment.h"
#include "storm/models/sparse/Pomdp.h"

namespace storm::logic {
class Formula;
}

namespace storm::pomdp::api {

/**
 * Approximates an unbounded reachability probability or expected reachability reward by unfolding the belief MDP.
 *
 * The model, formula, and options are left unchanged. Preparation canonicalizes the POMDP if needed (requiring choice labels),
 * refines observations to identify targets, makes until-failure states absorbing, and computes frontier value bounds.
 * Exactly one initial state and an explicit min/max optimization direction are required. Only non-nested formulae without
 * thresholds are supported; the result is quantitative. Solver precision and arithmetic follow the supplied environment
 * and numeric types.
 *
 * The value is a lower bound for maximization and an upper bound for minimization. Without clipping, complete exploration
 * gives the unabstracted belief MDP value, subject to numerical model checking. Completion with clipping does not imply exactness.
 *
 * @param options All unfolding settings. Unset size/time limits mean unlimited exploration; no heuristic budget is selected.
 * maxExplorationSize counts discovered beliefs, not transition depth, and is checked between expansions, so one expansion's
 * successors can exceed it. Zero uses preprocessing cut-offs without expanding any belief. maxExplorationTime is in seconds
 * and applies to exploration, not preprocessing or MDP verification. Building choice labels requires input choice labels,
 * even for an already canonical POMDP; otherwise disable buildChoiceLabeling. When clipping is enabled, clippingResolutions
 * must contain one positive entry per input observation. Observations introduced by target refinement inherit their original
 * observation's resolution.
 *
 * The supported (POMDP, belief, belief MDP) value types are (double, double, double), (double, RationalNumber, double),
 * (RationalNumber, RationalNumber, RationalNumber), (RationalNumber, RationalNumber, double), and (double, double, RationalNumber).
 *
 * Example:
 * @code
 * storm::pomdp::beliefs::BeliefBasedModelCheckerOptions<double> options;
 * options.maxExplorationSize = 1000;
 * options.maxExplorationTime = 10;
 * auto result = storm::pomdp::api::verifyWithBeliefUnfolding(env, pomdp, formula, options);
 * @endcode
 */
template<typename PomdpValueType, typename BeliefValueType = PomdpValueType, typename BeliefMdpValueType = PomdpValueType>
beliefs::BeliefBasedModelCheckerResult<BeliefMdpValueType> verifyWithBeliefUnfolding(
    storm::Environment const& env, storm::models::sparse::Pomdp<PomdpValueType> const& pomdp, storm::logic::Formula const& formula,
    beliefs::BeliefBasedModelCheckerOptions<BeliefMdpValueType> const& options);

/** Uses a default Environment; otherwise identical to the environment-taking overload. */
template<typename PomdpValueType, typename BeliefValueType = PomdpValueType, typename BeliefMdpValueType = PomdpValueType>
beliefs::BeliefBasedModelCheckerResult<BeliefMdpValueType> verifyWithBeliefUnfolding(
    storm::models::sparse::Pomdp<PomdpValueType> const& pomdp, storm::logic::Formula const& formula,
    beliefs::BeliefBasedModelCheckerOptions<BeliefMdpValueType> const& options) {
    return verifyWithBeliefUnfolding<PomdpValueType, BeliefValueType, BeliefMdpValueType>(storm::Environment(), pomdp, formula, options);
}

/**
 * Runs unfolding with default options and the supplied discovered-belief budget.
 *
 * Uses FIFO exploration and builds choice labels. Clipping and zero-gap cuts are disabled, and there is no exploration
 * time limit. Preparation, budget interpretation, result interpretation, and numeric support match the options overload.
 *
 * @code
 * auto result = storm::pomdp::api::verifyWithBeliefUnfolding(env, pomdp, formula, 1000);
 * @endcode
 */
template<typename PomdpValueType, typename BeliefValueType = PomdpValueType, typename BeliefMdpValueType = PomdpValueType>
beliefs::BeliefBasedModelCheckerResult<BeliefMdpValueType> verifyWithBeliefUnfolding(storm::Environment const& env,
                                                                                     storm::models::sparse::Pomdp<PomdpValueType> const& pomdp,
                                                                                     storm::logic::Formula const& formula, uint64_t maxExplorationSize);

/** Uses a default Environment; otherwise identical to the environment-taking overload. */
template<typename PomdpValueType, typename BeliefValueType = PomdpValueType, typename BeliefMdpValueType = PomdpValueType>
beliefs::BeliefBasedModelCheckerResult<BeliefMdpValueType> verifyWithBeliefUnfolding(storm::models::sparse::Pomdp<PomdpValueType> const& pomdp,
                                                                                     storm::logic::Formula const& formula, uint64_t maxExplorationSize) {
    return verifyWithBeliefUnfolding<PomdpValueType, BeliefValueType, BeliefMdpValueType>(storm::Environment(), pomdp, formula, maxExplorationSize);
}

/**
 * Runs unfolding with the CLI size heuristic: prepared POMDP states times maximum states sharing an observation.
 *
 * Preparation, result interpretation, and numeric support match verifyWithBeliefUnfolding. options.maxExplorationSize
 * must be unset; use the explicit-budget function to select a size. Other options, including a time limit, are honored.
 * Completion with clipping does not imply exactness.
 *
 * @param options Additional exploration settings. Building choice labels requires input choice labels, even for an already
 * canonical POMDP; otherwise disable buildChoiceLabeling. When clipping is enabled, clippingResolutions must contain one
 * positive entry per input observation; observations introduced by target refinement inherit their original observation's resolution.
 *
 * @code
 * auto result = storm::pomdp::api::verifyWithHeuristicBeliefUnfolding(pomdp, formula);
 * storm::pomdp::beliefs::BeliefBasedModelCheckerOptions<double> options;
 * options.maxExplorationTime = 10; // seconds; applies to exploration, not preprocessing or MDP verification
 * auto limited = storm::pomdp::api::verifyWithHeuristicBeliefUnfolding(env, pomdp, formula, options);
 * @endcode
 */
template<typename PomdpValueType, typename BeliefValueType = PomdpValueType, typename BeliefMdpValueType = PomdpValueType>
beliefs::BeliefBasedModelCheckerResult<BeliefMdpValueType> verifyWithHeuristicBeliefUnfolding(
    storm::Environment const& env, storm::models::sparse::Pomdp<PomdpValueType> const& pomdp, storm::logic::Formula const& formula,
    beliefs::BeliefBasedModelCheckerOptions<BeliefMdpValueType> const& options = {});

/** Uses a default Environment; otherwise identical to the environment-taking overload. */
template<typename PomdpValueType, typename BeliefValueType = PomdpValueType, typename BeliefMdpValueType = PomdpValueType>
beliefs::BeliefBasedModelCheckerResult<BeliefMdpValueType> verifyWithHeuristicBeliefUnfolding(
    storm::models::sparse::Pomdp<PomdpValueType> const& pomdp, storm::logic::Formula const& formula,
    beliefs::BeliefBasedModelCheckerOptions<BeliefMdpValueType> const& options = {}) {
    return verifyWithHeuristicBeliefUnfolding<PomdpValueType, BeliefValueType, BeliefMdpValueType>(storm::Environment(), pomdp, formula, options);
}

/**
 * Approximates an unbounded reachability probability or expected reachability reward by Freudenthal discretization.
 *
 * Preparation and numeric support match verifyWithBeliefUnfolding. The value is an upper bound for maximization and a
 * lower bound for minimization. Complete exploration describes the discretized MDP and does not imply exactness.
 *
 * @param resolution Positive integer grid resolution (default 2).
 * @param useDynamic Select per-belief resolutions using dynamic triangulation (default false).
 * @param options Exploration settings. Size/time limits and clipping are not supported by discretization.
 *
 * @code
 * auto result = storm::pomdp::api::verifyWithBeliefDiscretization(env, pomdp, formula, 10, true);
 * @endcode
 */
template<typename PomdpValueType, typename BeliefValueType = PomdpValueType, typename BeliefMdpValueType = PomdpValueType>
beliefs::BeliefBasedModelCheckerResult<BeliefMdpValueType> verifyWithBeliefDiscretization(
    storm::Environment const& env, storm::models::sparse::Pomdp<PomdpValueType> const& pomdp, storm::logic::Formula const& formula, uint64_t resolution = 2,
    bool useDynamic = false, beliefs::BeliefBasedModelCheckerOptions<BeliefMdpValueType> const& options = {});

/** Uses a default Environment; otherwise identical to the environment-taking overload. */
template<typename PomdpValueType, typename BeliefValueType = PomdpValueType, typename BeliefMdpValueType = PomdpValueType>
beliefs::BeliefBasedModelCheckerResult<BeliefMdpValueType> verifyWithBeliefDiscretization(
    storm::models::sparse::Pomdp<PomdpValueType> const& pomdp, storm::logic::Formula const& formula, uint64_t resolution = 2, bool useDynamic = false,
    beliefs::BeliefBasedModelCheckerOptions<BeliefMdpValueType> const& options = {}) {
    return verifyWithBeliefDiscretization<PomdpValueType, BeliefValueType, BeliefMdpValueType>(storm::Environment(), pomdp, formula, resolution, useDynamic,
                                                                                               options);
}

}  // namespace storm::pomdp::api
