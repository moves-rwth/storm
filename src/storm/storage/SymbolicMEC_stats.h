#ifndef STORM_STORAGE_SYMBOLICMEC_STATS_H
#define STORM_STORAGE_SYMBOLICMEC_STATS_H

#include <stack>
#include <unordered_map>
#include "storm/storage/dd/DdType.h"
#include "storm/storage/dd/Bdd.h"
#include "storm/storage/SymbolicSCCDecomposition_stats.h"

namespace symbolicMEC_stats {

template<storm::dd::DdType Type>
struct StateActionPair {
    storm::dd::Bdd<Type> states;
    storm::dd::Bdd<Type> actions; // [rmnt]: Actions is really state-action pairs.

    StateActionPair<Type>& operator|=(StateActionPair<Type> const & other)  {
        states  |= other.states;
        actions |= other.actions;
        return *this;
    }
};

// [rmnt] Changed type of scc argument because the earlier one was giving a compiler error.
// [rmnt] TODO review this function, seems shady.
template<storm::dd::DdType Type>
static bool isTrivialSccWithoutSelfEdge_stats(storm::dd::Bdd<Type> const & scc,
                                        storm::dd::Bdd<Type> const & transitionsWithActions,
                                        std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const & metaVariablesRowColumnPairs,
                                        uint_fast64_t &countSymbolicOps) {
    bool isTrivialScc = (1 == scc.getNonZeroCount()); // [rmnt] TODO : Check if this should be counted as a symbolic op
    countSymbolicOps++;
    if (! isTrivialScc)
        return false;
    bool noSelfEdge = (transitionsWithActions && scc && scc.swapVariables(metaVariablesRowColumnPairs)).isZero();
    return noSelfEdge;
}


/* Given a set of vertices T, the random attractor Attr_R(T)
 * is a set of vertices consisting of
 * (1) T,
 * (2) random vertices with an edge to some vertex in Attr_R(T),
 * (3) player-1 vertices with all outgoing edges in Attr_R(T).
 */
template<storm::dd::DdType Type>
static StateActionPair<Type> computeRandomAttractor_stats(storm::dd::Bdd<Type> const & actionsToApplyOn,
                                                    storm::dd::Bdd<Type> const & allStates,
                                                    storm::dd::Bdd<Type> const & transitionsWithActions,
                                                    std::set<storm::expressions::Variable> const & metaVariablesColumn,
                                                    std::set<storm::expressions::Variable> const & metaVariablesActions,
                                                    std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const & metaVariablesRowColumnPairs,
                                                    uint_fast64_t &countSymbolicOps) {
    StateActionPair<Type> nextSet = { .states = allStates.getDdManager().getBddZero(), .actions = actionsToApplyOn };
    StateActionPair<Type> currentSet;
    do {
        currentSet = nextSet;

        // Vertices
        storm::dd::Bdd<Type> actionsCannotIntoCurrent = ( transitionsWithActions && ( ! currentSet.actions)
        ).existsAbstract(metaVariablesColumn); // [rmnt]: All (s,a) pairs which aren't included in currentSet
        countSymbolicOps++;
        storm::dd::Bdd<Type> newVertices = currentSet.actions.existsAbstract(metaVariablesActions) 
        && ( ! (actionsCannotIntoCurrent.existsAbstract(metaVariablesActions)));
        // [rmnt]: All states s such that some (s,a1) in currentSet and all (s,a) pairs in currentSet
        countSymbolicOps++;
        nextSet.states = (currentSet.states || newVertices) && allStates;

        // Actions [rmnt] Actions is really state-action pairs.
        storm::dd::Bdd<Type> currentVerticesAsColumn = nextSet.states.swapVariables(metaVariablesRowColumnPairs);
        storm::dd::Bdd<Type> actionsCanIntoCurrentVertices = (transitionsWithActions && currentVerticesAsColumn).existsAbstract(metaVariablesColumn);
        countSymbolicOps++;
        nextSet.actions = currentSet.actions || actionsCanIntoCurrentVertices;
    } while (currentSet.states != nextSet.states);
    return currentSet;
}

// For a set of states S,
// return all actions which have a non-zero-probability of leaving S.
template<storm::dd::DdType Type>
static storm::dd::Bdd<Type> ROut_stats(storm::dd::Bdd<Type> const & sccStates,
                                 storm::dd::Bdd<Type> const & transitionsWithActions,
                                 std::set<storm::expressions::Variable> const & metaVariablesColumn,
                                 std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const & metaVariablesRowColumnPairs,
                                 uint_fast64_t &countSymbolicOps) {
    storm::dd::Bdd<Type> transitionsFromSccToOutside = sccStates && transitionsWithActions && ( ! sccStates.swapVariables(metaVariablesRowColumnPairs));
    storm::dd::Bdd<Type> actionsLeavingScc = transitionsFromSccToOutside.existsAbstract(metaVariablesColumn);
    countSymbolicOps++;
    return actionsLeavingScc;
}

// As described in
// "Symbolic algorithms for graphs and Markov decision processes with fairness objectives"
template<storm::dd::DdType Type, typename ValueType>
std::vector<storm::dd::Bdd<Type>> symbolicMECDecompositionNaive_stats(storm::dd::Bdd<Type> const & allStates,
                                                                storm::dd::Bdd<Type> const & transitionsWithActions,
                                                                std::set<storm::expressions::Variable> const & metaVariablesRow,
                                                                std::set<storm::expressions::Variable> const & metaVariablesColumn,
                                                                std::set<storm::expressions::Variable> const & metaVariablesActions,
                                                                std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const & metaVariablesRowColumnPairs,
                                                                uint_fast64_t &countSymbolicOps) {
    storm::dd::Bdd<Type> workingCopyTransitionsWithActions(transitionsWithActions);
    std::vector<storm::dd::Bdd<Type>> result{};
    std::stack<storm::dd::Bdd<Type>> mecCandidates{};
    for (const auto& scc : symbolicSCC_stats::decomposition_stats<Type, ValueType>(allStates, workingCopyTransitionsWithActions.existsAbstract(metaVariablesActions),
                                                                       metaVariablesRow, metaVariablesColumn, countSymbolicOps)) {
        mecCandidates.push(scc);
    }
    while ( ! mecCandidates.empty()) {
        storm::dd::Bdd<Type> scc = mecCandidates.top();
        mecCandidates.pop();

        if (isTrivialSccWithoutSelfEdge_stats(scc, workingCopyTransitionsWithActions, metaVariablesRowColumnPairs, countSymbolicOps)) {
            continue;
        }

        storm::dd::Bdd<Type> sccROut = ROut_stats(scc, workingCopyTransitionsWithActions,
        metaVariablesColumn, metaVariablesRowColumnPairs, countSymbolicOps);
        if (sccROut.isZero()) {
            result.template emplace_back(scc);
        } else {
            StateActionPair<Type> attractor = computeRandomAttractor_stats(sccROut, scc, workingCopyTransitionsWithActions,
            metaVariablesColumn, metaVariablesActions, metaVariablesRowColumnPairs, countSymbolicOps);
            workingCopyTransitionsWithActions &= !attractor.actions;
            countSymbolicOps++; // [rmnt] For the existsAbstract call in args to scc decomp
            for (auto const& subScc : symbolicSCC_stats::decomposition_stats<Type, ValueType>(
                     scc && !attractor.states, workingCopyTransitionsWithActions.existsAbstract(metaVariablesActions),
                     metaVariablesRow, metaVariablesColumn, countSymbolicOps)) {
                mecCandidates.push(subScc);
            }
        }
        std::cout << "DEBUG Transition BDD stats: "
                  << workingCopyTransitionsWithActions.getNonZeroCount() << " Transitions (" << workingCopyTransitionsWithActions.getNodeCount() << " nodes), "
                  << workingCopyTransitionsWithActions.existsAbstract(metaVariablesActions).getNonZeroCount() << " Transitions (" << workingCopyTransitionsWithActions.existsAbstract(metaVariablesActions).getNodeCount() << " nodes)"
                  << std::endl;
    }
    return result;
}

// As described in
// "Symbolic Algorithms for Graphs and Markov Decision Processes with Fairness Objectives"
// Slightly simplified we only care about the bottom scc
template<storm::dd::DdType Type, typename ValueType>
storm::dd::Bdd<Type> LockStepSearch_stats(storm::dd::Bdd<Type> const & states, // S
                                    storm::dd::Bdd<Type> const & statesWithRemovedEdges, // T_S
                                    storm::dd::Bdd<Type> const & transitionsWithoutActions,
                                    std::set<storm::expressions::Variable> const & metaVariablesRow,
                                    std::set<storm::expressions::Variable> const & metaVariablesColumn,
                                    uint_fast64_t &countSymbolicOps) {
    storm::dd::Bdd<Type> ts = storm::dd::Bdd<Type>(statesWithRemovedEdges);
    std::unordered_map<storm::dd::Bdd<Type>, storm::dd::Bdd<Type>> c{};
    while ( ! ts.isZero()) {
        storm::dd::Bdd<Type> v = pick_stats<Type, ValueType>(ts, countSymbolicOps);
        ts &= ! v;
        c[v] = storm::dd::Bdd<Type>(v);
    }

    ts = storm::dd::Bdd<Type>(statesWithRemovedEdges);
    while (true) {
        for (auto it = c.cbegin(); it != c.cend(); ) {
            storm::dd::Bdd<Type> t = storm::dd::Bdd<Type>(it->first);
            storm::dd::Bdd<Type> ct = storm::dd::Bdd<Type>(it->second);
            storm::dd::Bdd<Type> ctNew = ct || post_stats(ct, states, transitionsWithoutActions, metaVariablesRow, metaVariablesColumn, countSymbolicOps);
            if ((ctNew && ts).getNonZeroCount() > 1) { // [rmnt] there was another state in T_S in the same SCC as t, so we can ignore t.
                ts &= ! t;
                c.erase(it++);
            } else {
                if (ctNew == ct) {
                    return ctNew;
                }
                c[t] = ctNew;
                ++it;
            }
        }
    }
}

// As described in
// "Symbolic Algorithms for Graphs and Markov Decision Processes with Fairness Objectives"
// Uses a lockstep search
template<storm::dd::DdType Type, typename ValueType>
std::vector<storm::dd::Bdd<Type>> symbolicMECDecompositionLockstep_stats(storm::dd::Bdd<Type> const & allStates,
                                                                   storm::dd::Bdd<Type> const & transitionsWithActions,
                                                                   std::set<storm::expressions::Variable> const & metaVariablesRow,
                                                                   std::set<storm::expressions::Variable> const & metaVariablesColumn,
                                                                   std::set<storm::expressions::Variable> const & metaVariablesActions,
                                                                   std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const & metaVariablesRowColumnPairs,
                                                                   uint_fast64_t &countSymbolicOps) {
    struct MecCandidate {
        storm::dd::Bdd<Type> states; // In paper: S
        storm::dd::Bdd<Type> statesWithRemovedEdge; // In Paper: T_s
    };

    // Helper function for better readability
    auto hasAtLeastOneEdge_stats =
        [metaVariablesActions, metaVariablesRow, metaVariablesColumn, metaVariablesRowColumnPairs]
        (storm::dd::Bdd<Type> scc, storm::dd::Bdd<Type> transitionsWithActions, uint_fast64_t &countSymbolicOps) {
        countSymbolicOps++; // [rmnt] For the exists abstract in the arguments
        // Paper suggestion
        return ! (post_stats(scc, scc, transitionsWithActions.existsAbstract(metaVariablesActions), metaVariablesRow, metaVariablesColumn, countSymbolicOps)).isZero();
    };

    countSymbolicOps++; // [rmnt] exists
    countSymbolicOps++; // [rmnt] getNonZeroCount
    uint_fast64_t m = (transitionsWithActions.existsAbstract(metaVariablesActions) && allStates).getNonZeroCount();
    storm::dd::Bdd<Type> workingCopyTransitionsWithActions(transitionsWithActions);
    std::vector<storm::dd::Bdd<Type>> result{}; // In paper: "goodC"
    std::stack<MecCandidate> mecCandidates{}; // In paper: X
    countSymbolicOps++; // [rmnt] For the exists abstract in the arguments
    for (const auto & scc : symbolicSCC_stats::decomposition_stats<Type, ValueType>(allStates, workingCopyTransitionsWithActions.existsAbstract(metaVariablesActions), metaVariablesRow, metaVariablesColumn, countSymbolicOps)) {
        MecCandidate pair = { scc, scc.getDdManager().getBddZero() };
        mecCandidates.push(pair);
    }
    while ( ! mecCandidates.empty()) {
        MecCandidate currentCandidate = mecCandidates.top();
        storm::dd::Bdd<Type> scc = currentCandidate.states;
        storm::dd::Bdd<Type> sccTs = currentCandidate.statesWithRemovedEdge;
        mecCandidates.pop();

        storm::dd::Bdd<Type> sccROut = ROut_stats(scc, workingCopyTransitionsWithActions, metaVariablesColumn, metaVariablesRowColumnPairs, countSymbolicOps);
        StateActionPair<Type> randomAttractor = computeRandomAttractor_stats(sccROut, scc, workingCopyTransitionsWithActions, metaVariablesColumn, metaVariablesActions, metaVariablesRowColumnPairs, countSymbolicOps);
        scc &= ! randomAttractor.states;
        workingCopyTransitionsWithActions &= ! randomAttractor.actions;
        countSymbolicOps++;
        sccTs = (sccTs || randomAttractor.actions.existsAbstract(metaVariablesActions)) && scc;
        if (hasAtLeastOneEdge_stats(scc, workingCopyTransitionsWithActions, countSymbolicOps)) {
            if (sccTs.isZero()) {
                result.template emplace_back(scc);
            } else if (sccTs.getNonZeroCount() >= sqrt(m)) {
                countSymbolicOps++; // [rmnt] for the else if condition
                countSymbolicOps++; // [rmnt] for the exists below
                std::vector<storm::dd::Bdd<Type>> subSccs = symbolicSCC_stats::decomposition_stats<Type, ValueType>(
                    scc, workingCopyTransitionsWithActions.existsAbstract(metaVariablesActions), metaVariablesRow, metaVariablesColumn, countSymbolicOps);
                if (subSccs.size() == 1) {
                    result.template emplace_back(scc);
                } else {
                    for (auto const& subScc : subSccs) {
                        MecCandidate toProcess = {subScc, scc.getDdManager().getBddZero()};
                        mecCandidates.push(toProcess);
                    }
                }
            } else {
                countSymbolicOps++; // [rmnt] for the else if condition
                // bottomScc is "C" in paper
                storm::dd::Bdd<Type> bottomScc = LockStepSearch_stats<Type, ValueType>(scc, sccTs,
                                                                                 workingCopyTransitionsWithActions.existsAbstract(metaVariablesActions),
                                                                                 metaVariablesRow, metaVariablesColumn, countSymbolicOps);
                if (hasAtLeastOneEdge_stats(bottomScc, workingCopyTransitionsWithActions, countSymbolicOps)) {
                    result.template emplace_back(bottomScc);
                }

                countSymbolicOps++; // [rmnt] for the exists below
                storm::dd::Bdd<Type> statesLeadingIntoBottomScc = pre_stats(bottomScc, scc, workingCopyTransitionsWithActions.existsAbstract(metaVariablesActions), metaVariablesRow, metaVariablesColumn, countSymbolicOps);
                mecCandidates.emplace((MecCandidate) {
                    scc && ( ! bottomScc),
                    (scc && ( ! bottomScc)) && (statesLeadingIntoBottomScc || sccTs),
                });
            }
        }
    }
    return result;
}


template<storm::dd::DdType Type, typename ValueType>
std::vector<storm::dd::Bdd<Type>> symbolicMECDecompositionInterleave_stats(storm::dd::Bdd<Type> const & allStates,
                                                                storm::dd::Bdd<Type> const & transitionsWithActions,
                                                                std::set<storm::expressions::Variable> const & metaVariablesRow,
                                                                std::set<storm::expressions::Variable> const & metaVariablesColumn,
                                                                std::set<storm::expressions::Variable> const & metaVariablesActions,
                                                                std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const & metaVariablesRowColumnPairs,
                                                                uint_fast64_t &countSymbolicOps) {
    return std::vector< storm::dd::Bdd<Type> > {};
}

} // namespace symbolicMEC_stats

#endif