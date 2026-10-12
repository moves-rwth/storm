#pragma once

#include <map>
#include <memory>
#include <utility>
#include <vector>

#include "storm-pars/scp/AffinePMdpDecomposition.h"
#include "storm-pars/scp/ScpOptions.h"
#include "storm/adapters/RationalFunctionForward.h"
#include "storm/environment/Environment.h"
#include "storm/solver/LpSolver.h"
#include "storm/storage/BitVector.h"
#include "storm/storage/expressions/Variable.h"

namespace storm::pars::scp {

/*!
 * Which side of the threshold a witness valuation is being searched for, under the *robust*
 * (universally-quantified-over-schedulers) reading of the property -- matching storm's own
 * default for an unqualified bounded formula on a nondeterministic model
 * (CheckTask::updateOperatorInformation: Less/LessEqual -> Maximize scheduler, Greater/GreaterEqual
 * -> Minimize scheduler):
 *
 *   AtLeast (threshold is Greater/GreaterEqual) <-> minimal reachability probability (Pmin)
 *   AtMost  (threshold is Less/LessEqual)       <-> maximal reachability probability (Pmax)
 *
 * "AtLeast" means: even the worst-case (minimizing) scheduler clears the threshold. "AtMost" means:
 * even the worst-case (maximizing) scheduler stays under it. For a Dtmc there is no scheduler
 * choice, but the same pairing is still used (see the class-level doc comment on ScpLpEncoder for
 * why the LP needs *a* consistent choice regardless).
 */
enum class ThresholdDirection { AtLeast, AtMost };

/*!
 * Which kind of property the LP is being built for: an unbounded reachability *probability*
 * (states have a natural [0,1] domain; a "known" successor value of exactly 1 is possible, see
 * ScpLpEncoder's oneStates parameter), or an unbounded reachability *reward* (states are
 * non-negative but otherwise domain-unbounded above; there is no "known value 1" case -- the
 * reward-model analog of a state whose value is known-but-not-zero is a state with *infinite*
 * expected reward, which is never a valid successor of any row the encoder is asked to build a
 * constraint for in the first place, see ScpLpEncoder's excludedRows parameter).
 */
enum class PropertyKind { Probability, Reward };

/*!
 * The LP built for one SCP iteration, together with the handles needed to read a solution back
 * out of it once solver->optimize() has been called.
 *
 * stateVariables is indexed by state; entries for Prob0/Prob1 states are default-constructed
 * (never touched by the encoder) since those states are encoded as constants, not LP variables --
 * callers already know their value (0 or 1 respectively) without consulting the LP.
 */
struct EncodedLp {
    std::unique_ptr<storm::solver::LpSolver<double>> solver;
    std::vector<storm::expressions::Variable> stateVariables;
    std::map<storm::RationalFunctionVariable, storm::expressions::Variable> parameterVariables;
};

/*!
 * Builds the linearized-Bellman LP subproblem for one SCP iteration (paper Fig. 5 / Eq. 30-38;
 * see the implementation plan for the full derivation).
 *
 * Everything that does not change between iterations (the affine decomposition, the row-group
 * structure, Prob0/Prob1, the parameter bounds, the threshold) is fixed at construction; encode()
 * is a pure function of the current linearization point and trust region radius, rebuilding the
 * LP from scratch on every call (no incremental/warm-started LP for V1, see the plan).
 *
 * IMPORTANT -- the Bellman inequality direction (and hence which Prob0/Prob1 qualitative pair the
 * caller must supply) depends on ThresholdDirection, by LP duality:
 *
 *   AtMost:  "p_s >= rhs - tau_s" for every action, objective MINIMIZES p_{s_I}
 *            -> converges to the *least supersolution* = Pmax, tightly, as the accuracy-preserving
 *               choice for that extremal. Caller must supply the Pmax-qualitative pair
 *               (performProb01Max for an Mdp, performProb01 for a Dtmc).
 *   AtLeast: "p_s <= rhs + tau_s" for every action, objective MAXIMIZES p_{s_I}
 *            -> converges to the *greatest subsolution* = Pmin, tightly. Caller must supply the
 *               Pmin-qualitative pair (performProb01Min for an Mdp, performProb01 for a Dtmc).
 *
 * Getting this backwards (e.g. always using ">=" regardless of direction) does not just compute
 * the wrong extremal -- for the direction that then pairs a "maximize" objective with a
 * lower-bound-only constraint, p_{s_I} becomes free to inflate to its own trust-region ceiling
 * with zero cost, completely independent of what the parameters actually support: a
 * self-consistent-looking but meaningless LP solution. The pairing above is load-bearing, not an
 * arbitrary convention.
 */
class ScpLpEncoder {
   public:
    /*!
     * @param decomposition The affine decomposition of the model (and, for a reward property, of
     * the relevant reward model) -- must outlive this encoder.
     * @param rowGroupIndices The model's transition matrix row-group boundaries (state -> rows).
     * @param zeroStates States with known value 0: Prob0 (probability property) or Rew0, i.e. the
     * target states themselves (reward property). Excluded from LP-variable creation; contribute
     * nothing when a successor.
     * @param oneStates States with known value 1: Prob1 (probability property only -- pass an
     * empty BitVector for a reward property, which has no such state). Excluded from LP-variable
     * creation; when a successor, contribute their exact affine transition value (never
     * linearized, since multiplying by the *known constant* 1 introduces no bilinear term).
     * @param infiniteStates States with known value infinity: RewInf (reward property only -- pass
     * an empty BitVector for a probability property). Excluded from LP-variable creation and from
     * having a Bellman constraint built for them; by construction of `excludedRows` below, never
     * actually referenced as a successor of any row this encoder builds a constraint for.
     * @param excludedRows Rows to skip entirely when building Bellman constraints, beyond the
     * per-state exclusion above -- needed because a reward property's "value known" classification
     * is graph-based per *state*, but an individual action (row) of an otherwise-included MDP
     * state can still lead to an infiniteStates successor and must be excluded on its own (an MDP
     * state can have both safe and unsafe actions; a DTMC state cannot, so this is always empty
     * for DTMCs). Pass an all-clear BitVector (sized to the total row count) for a probability
     * property, which has no such case.
     * @param initialState The state whose value is compared against the threshold. Must not be a
     * zero/one/infinite state -- callers should short-circuit before building an LP in that
     * degenerate case, since the answer is then already known without solving anything.
     * @param parameterBounds The (lower, upper) bound of each parameter (i.e. the outer search
     * region -- the trust region is intersected with this, never outside it).
     * @param threshold The threshold to compare the initial state's value against.
     * @param direction Whether a valuation with value >= or <= threshold is being searched for.
     * @param propertyKind Probability (state variables bounded above by 1; no reward term in the
     * Bellman RHS) or Reward (state variables unbounded above; the Bellman RHS additionally
     * includes the row's affine reward, from decomposition.getReward()).
     */
    ScpLpEncoder(AffinePMdpDecomposition const& decomposition, std::vector<uint64_t> rowGroupIndices, storm::storage::BitVector zeroStates,
                 storm::storage::BitVector oneStates, storm::storage::BitVector infiniteStates, storm::storage::BitVector excludedRows, uint64_t initialState,
                 std::map<storm::RationalFunctionVariable, std::pair<double, double>> parameterBounds, double threshold, ThresholdDirection direction,
                 PropertyKind propertyKind, ScpOptions options);

    /*!
     * Builds the LP for one SCP iteration, linearized around (valuation, stateValues) with the
     * given trust region radius. Does not call solver->optimize() -- that is the caller's job.
     *
     * @param env The environment to build the LP solver with (e.g. solver-type selection); the
     * same one the caller uses for the exact-check model checker, so a caller's customized LP
     * solver choice is actually respected rather than silently falling back to a default.
     * @param valuation The current parameter linearization point; must contain every parameter
     * occurring in the model.
     * @param stateValues The current per-state linearization point (index = state); only entries
     * for "maybe" states (not zeroStates/oneStates/infiniteStates) are read.
     * @param trustRegionRadius The multiplicative trust region radius (>= 1); see ScpOptions.
     */
    EncodedLp encode(storm::Environment const& env, std::map<storm::RationalFunctionVariable, double> const& valuation, std::vector<double> const& stateValues,
                     double trustRegionRadius) const;

   private:
    AffinePMdpDecomposition const& decomposition;
    std::vector<uint64_t> rowGroupIndices;
    storm::storage::BitVector zeroStates;
    storm::storage::BitVector oneStates;
    storm::storage::BitVector infiniteStates;
    storm::storage::BitVector excludedRows;
    uint64_t initialState;
    std::map<storm::RationalFunctionVariable, std::pair<double, double>> parameterBounds;
    double threshold;
    ThresholdDirection direction;
    PropertyKind propertyKind;
    ScpOptions options;
};

}  // namespace storm::pars::scp
